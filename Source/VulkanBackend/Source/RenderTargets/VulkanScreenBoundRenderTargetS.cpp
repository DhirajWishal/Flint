// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/VulkanScreenBoundRenderTargetS.h"
#include "VulkanBackend/RenderTargets/Pipelines/VulkanGraphicsPipeline.h"
#include "VulkanBackend/VulkanCommandBufferManager.h"
#include "VulkanBackend/VulkanCommandBuffer.h"
#include "VulkanBackend/VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanScreenBoundRenderTargetS::Initialize(Backend::Device* pDevice, const Vector2& extent, UI32 bufferCount)
		{
			this->pDevice = pDevice;
			this->mExtent = extent;
			this->pCommandBufferManager = new VulkanCommandBufferManager();

			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			this->mBufferCount = pvDevice->FindSupporterBufferCount(bufferCount);

			pCommandBufferManager->CreateBuffers(pDevice, static_cast<UI32>(mBufferCount));
			vSwapChain.Initialize(pvDevice, extent, static_cast<UI32>(mBufferCount));
			vColorBuffer.Initialize(pvDevice, extent, vSwapChain.GetFormat(), static_cast<UI32>(mBufferCount));
			vDepthBuffer.Initialize(pvDevice, extent, static_cast<UI32>(mBufferCount));

			CreateRenderPass(pvDevice, { &vColorBuffer, &vDepthBuffer, &vSwapChain }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			CreateFrameBuffer(pvDevice, { &vColorBuffer, &vDepthBuffer, &vSwapChain }, mExtent, static_cast<UI32>(mBufferCount));

			InitializeSyncObjects(pvDevice, static_cast<UI32>(this->mBufferCount));
		}

		void VulkanScreenBoundRenderTargetS::Terminate()
		{
			vSwapChain.Terminate();
			vColorBuffer.Terminate();
			vDepthBuffer.Terminate();

			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			DestroyRenderPass(pvDevice);
			DestroyFrameBuffers(pvDevice);
			TerminateSyncObjects(pvDevice);

			DestroyChildCommandBuffers();

			pCommandBufferManager->Terminate();
			delete pCommandBufferManager;
		}

		Backend::GraphicsPipeline* VulkanScreenBoundRenderTargetS::CreateGraphicsPipeline(const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec)
		{
			VulkanGraphicsPipeline* pPipeline = new VulkanGraphicsPipeline();
			pPipeline->Initialize(this, shaderDigests, spec);
			return pPipeline;
		}

		UI32 VulkanScreenBoundRenderTargetS::PrepareToDraw()
		{
			VulkanDevice* pvDevice = GetDevice()->Derive<VulkanDevice>();
			FLINT_VK_ASSERT(vkWaitForFences(pvDevice->GetLogicalDevice(), 1, &vInFlightFences[mFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()), "Failed to wait for fence!")
				//FLINT_VK_ASSERT(pvDevice->ResetFences({ vInFlightFences[mFrameIndex] }), "Failed to reset fence!")

				VkResult result = vkAcquireNextImageKHR(pvDevice->GetLogicalDevice(), GetSwapChain(), std::numeric_limits<UI32>::max(), vImageAvailables[mFrameIndex], VK_NULL_HANDLE, &mImageIndex);

			if (mDynamicDrawEntries.Size())
				SubmitSecondaryCommands();

			return mFrameIndex;
		}

		void VulkanScreenBoundRenderTargetS::SubmitCommand()
		{
			VulkanDevice* pvDevice = GetDevice()->Derive<VulkanDevice>();
			if (vImagesInFlightFences[mImageIndex] != VK_NULL_HANDLE)
				FLINT_VK_ASSERT(vkWaitForFences(pvDevice->GetLogicalDevice(), 1, &vImagesInFlightFences[mImageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()), "Failed to wait for fence!")

				vImagesInFlightFences[mImageIndex] = vInFlightFences[mFrameIndex];

			VkSemaphoreWaitFlags vWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			VkSubmitInfo vSI = {};
			vSI.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSI.pNext = VK_NULL_HANDLE;
			vSI.commandBufferCount = 1;
			vSI.pCommandBuffers = *dynamic_cast<VulkanCommandBuffer*>(const_cast<Backend::CommandBuffer*>(pCommandBufferManager->GetBuffer(mFrameIndex).get()));
			vSI.waitSemaphoreCount = 1;
			vSI.pWaitSemaphores = &vImageAvailables[mFrameIndex];
			vSI.signalSemaphoreCount = 1;
			vSI.pSignalSemaphores = &vRenderFinishes[mFrameIndex];
			vSI.pWaitDstStageMask = &vWaitStage;

			FLINT_VK_ASSERT(vkResetFences(pvDevice->GetLogicalDevice(), 1, &vInFlightFences[mFrameIndex]), "Failed to reset fence!")
				FLINT_VK_ASSERT(pvDevice->SubmitQueue(pvDevice->GetGraphicsQueue(), { vSI }, vInFlightFences[mFrameIndex]), "Failed to submit queue!")

				VkPresentInfoKHR vPI = {};
			vPI.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			vPI.pNext = VK_NULL_HANDLE;
			vPI.waitSemaphoreCount = 1;
			vPI.pWaitSemaphores = &vRenderFinishes[mFrameIndex];
			vPI.swapchainCount = 1;

			VkSwapchainKHR vSwapChain = GetSwapChain();
			vPI.pSwapchains = &vSwapChain;
			vPI.pImageIndices = &mImageIndex;

			FLINT_VK_ASSERT(vkQueuePresentKHR(pvDevice->GetTransferQueue(), &vPI), "Failed to present queue!")
				FLINT_VK_ASSERT(vkQueueWaitIdle(pvDevice->GetTransferQueue()), "Failed to wait idle till the queue is complete!")

				mFrameIndex++;
			if (mFrameIndex >= mBufferCount) mFrameIndex = 0;
		}

		void VulkanScreenBoundRenderTargetS::Bind(const std::shared_ptr<Backend::CommandBuffer>& pCommandBuffer)
		{
			VkClearValue pClearValues[2] = {};
			pClearValues[1].depthStencil = { 1.0f, 0 };
			pClearValues[0].color.float32[0] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[1] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[2] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[3] = 1.0f;

			VkRenderPassBeginInfo vRBI = {};
			vRBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vRBI.pNext = VK_NULL_HANDLE;
			vRBI.clearValueCount = 2;
			vRBI.pClearValues = pClearValues;
			vRBI.renderPass = vRenderPass;
			vRBI.framebuffer = vFrameBuffers[mFrameIndex];
			vRBI.renderArea.extent = { static_cast<UI32>(mExtent.x), static_cast<UI32>(mExtent.y) };

			vkCmdBeginRenderPass(*dynamic_cast<VulkanCommandBuffer*>(const_cast<Backend::CommandBuffer*>(pCommandBuffer.get())), &vRBI, VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanScreenBoundRenderTargetS::UnBind(const std::shared_ptr<Backend::CommandBuffer>& pCommandBuffer)
		{
			vkCmdEndRenderPass(*dynamic_cast<VulkanCommandBuffer*>(const_cast<Backend::CommandBuffer*>(pCommandBuffer.get())));
		}

		void VulkanScreenBoundRenderTargetS::SubmitSecondaryCommands()
		{
			auto& pSecondaryCommandBuffers = mChildCommandBuffers[0];
			pCommandBufferManager->UpdateChild(pSecondaryCommandBuffers.get(), this);

			auto& pCommandBuffer = pCommandBufferManager->GetBuffer(mImageIndex);

			std::vector<DrawEntry> dynamicVector;
			for (auto itr = mDynamicDrawEntries.Begin(); itr != mDynamicDrawEntries.End(); itr++)
				INSERT_INTO_VECTOR(dynamicVector, itr->second);

			VkClearValue pClearValues[2] = {};
			pClearValues[1].depthStencil = { 1.0f, 0 };
			pClearValues[0].color.float32[0] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[1] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[2] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[3] = 1.0f;

			VkRenderPassBeginInfo vRBI = {};
			vRBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vRBI.pNext = VK_NULL_HANDLE;
			vRBI.clearValueCount = 2;
			vRBI.pClearValues = pClearValues;
			vRBI.renderPass = vRenderPass;
			vRBI.framebuffer = vFrameBuffers[mImageIndex];
			vRBI.renderArea.extent = { static_cast<UI32>(mExtent.x), static_cast<UI32>(mExtent.y) };

			pCommandBuffer->BeginBufferRecording();
			auto pBuff = dynamic_cast<VulkanCommandBuffer*>(const_cast<Backend::CommandBuffer*>(pCommandBuffer.get()));
			vkCmdBeginRenderPass(*pBuff, &vRBI, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

			BakeDynamicCommands(dynamicVector, pSecondaryCommandBuffers);

			pCommandBuffer->ExecuteChildCommands(pSecondaryCommandBuffers->GetBuffers());

			UnBind(pCommandBuffer);
			pCommandBuffer->EndBufferRecording();
		}
	}
}

/*
VUID-VkViewport-width-01770(ERROR / SPEC): msgNum: -1542042715 - Validation Error: [ VUID-VkViewport-width-01770 ] Object 0: handle = 0x2637e7aecd8, type = VK_OBJECT_TYPE_COMMAND_BUFFER; | MessageID = 0xa4164ba5 | vkCmdSetViewport: pViewports[0].width (=0.000000) is not greater than 0.0. The Vulkan spec states: width must be greater than 0.0 (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-VkViewport-width-01770)
	Objects: 1
		[0] 0x2637e7aecd8, type: 6, name: NULL*/