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

			CreateRenderPass(pvDevice, { &vColorBuffer, &vDepthBuffer }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			CreateFrameBuffer(pvDevice, { &vSwapChain, &vDepthBuffer }, mExtent, static_cast<UI32>(mBufferCount));

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
			mFrameIndex++;
			if (mFrameIndex >= mBufferCount) mFrameIndex = 0;

			VulkanDevice* pvDevice = GetDevice()->Derive<VulkanDevice>();
			FLINT_VK_ASSERT(vkWaitForFences(pvDevice->GetLogicalDevice(), 1, &vInFlightFences[mFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()), "Failed to wait for fence!")
				FLINT_VK_ASSERT(pvDevice->ResetFences({ vInFlightFences[mFrameIndex] }), "Failed to reset fence!")

				VkResult result = vkAcquireNextImageKHR(pvDevice->GetLogicalDevice(), GetSwapChain(), std::numeric_limits<UI32>::max(), vRenderFinishes[mFrameIndex], vInFlightFences[mFrameIndex], &mFrameIndex);

			if (mDynamicDrawEntries.Size())
				SubmitSecondaryCommands();

			return mFrameIndex;
		}

		void VulkanScreenBoundRenderTargetS::SubmitCommand()
		{
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

			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
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
			//pSecondaryCommandBuffers->Reset();

			auto& pCommandBuffer = pCommandBufferManager->GetBuffer(mFrameIndex);

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
			vRBI.framebuffer = vFrameBuffers[mFrameIndex];
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
{ 17:25:23 } Vulkan Validation Layer (Validation): Validation Error: [ VUID-vkQueueSubmit-pWaitSemaphores-03238 ] Object 0: handle = 0x41ab840000000021, type = VK_OBJECT_TYPE_SEMAPHORE; Object 1: handle = 0x1a699d1f6b8, type = VK_OBJECT_TYPE_QUEUE; | MessageID = 0xb50452b0 | vkQueueSubmit: Queue VkQueue 0x1a699d1f6b8[] is waiting on pSubmits[0].pWaitSemaphores[0] (VkSemaphore 0x41ab840000000021[]) that has no way to be signaled. The Vulkan spec states: All elements of the pWaitSemaphores member of all elements of pSubmits created with a VkSemaphoreType of VK_SEMAPHORE_TYPE_BINARY must reference a semaphore signal operation that has been submitted for execution and any semaphore signal operations on which it depends (if any) must have also been submitted for execution (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkQueueSubmit-pWaitSemaphores-03238)
{ 17:25:23 } Vulkan Validation Layer (Validation): Validation Error: [ VUID-vkAcquireNextImageKHR-semaphore-01286 ] Object 0: handle = 0x8483000000000025, type = VK_OBJECT_TYPE_SEMAPHORE; | MessageID = 0xe9e4b2a9 | vkAcquireNextImageKHR: Semaphore must not be currently signaled or in a wait state. The Vulkan spec states: If semaphore is not VK_NULL_HANDLE it must be unsignaled (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkAcquireNextImageKHR-semaphore-01286)
*/