// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend\RenderTargets\VulkanScreenBoundRenderTarget.h"
#include "VulkanBackend\RenderTargets\Pipelines\VulkanGraphicsPipeline.h"
#include "VulkanBackend\RenderTargets\Pipelines\VulkanPipelineResource.h"
#include "VulkanBackend\VulkanMacros.h"

#include "Core\Thread\ThreadUtilities.h"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanScreenBoundRenderTarget::VulkanScreenBoundRenderTarget(Backend::Device* pDevice, Backend::Display* pDisplay, UI64 bufferCount)
			: ScreenBoundRenderTarget(pDevice, pDisplay, bufferCount)
		{
			this->mBufferCount = pDisplay->GetAs<VulkanDisplay>()->FindSupporterBufferCount(pDevice->GetAs<VulkanDevice>(), static_cast<UI32>(bufferCount));
			this->mExtent = pDisplay->GetExtent();
			this->pCommandBufferList = std::make_unique<VulkanCommandBufferList>(pDevice, static_cast<UI32>(mBufferCount));

			vSwapChain.Initialize(pDevice->GetAs<VulkanDevice>(), pDisplay->GetAs<VulkanDisplay>(), mExtent, static_cast<UI32>(mBufferCount));
			vColorBuffer.Initialize(pDevice->GetAs<VulkanDevice>(), mExtent, vSwapChain.GetFormat(), static_cast<UI32>(mBufferCount));
			vDepthBuffer.Initialize(pDevice->GetAs<VulkanDevice>(), mExtent, static_cast<UI32>(mBufferCount));

			CreateRenderPass(pDevice->GetAs<VulkanDevice>(), { &vColorBuffer, &vDepthBuffer, &vSwapChain }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			CreateFrameBuffer(pDevice->GetAs<VulkanDevice>(), { &vColorBuffer, &vDepthBuffer, &vSwapChain }, mExtent, static_cast<UI32>(mBufferCount));

			InitializeSyncObjects(pDevice->GetAs<VulkanDevice>(), static_cast<UI32>(this->mBufferCount));
		}

		void VulkanScreenBoundRenderTarget::Terminate()
		{
			vSwapChain.Terminate();
			vColorBuffer.Terminate();
			vDepthBuffer.Terminate();

			DestroyRenderPass(pDevice->GetAs<VulkanDevice>());
			DestroyFrameBuffers(pDevice->GetAs<VulkanDevice>());
			TerminateSyncObjects(pDevice->GetAs<VulkanDevice>());

			//DestroyChildCommandBuffers();
		}

		void VulkanScreenBoundRenderTarget::BakeCommands()
		{
			VulkanCommandBufferList* pCommandBuffer = pCommandBufferList->GetAs<VulkanCommandBufferList>();
			for (UI64 counter = 0; counter < mBufferCount; counter++)
			{
				pCommandBuffer->BeginBuffer(counter);
				pCommandBuffer->BindRenderTarget(this);

				for (UI64 entryIndex = 0; entryIndex < mStaticDrawIndex; entryIndex++)
				{
					if (mStaticDrawEntries.find(entryIndex) == mStaticDrawEntries.end())
						continue;

					const auto& entry = mStaticDrawEntries[entryIndex];
					pCommandBuffer->BindVertexBuffer(entry.pVertexBuffer.lock().get());
					pCommandBuffer->BindIndexBuffer(entry.pIndexBuffer.lock().get());

					for (UI64 pipelineIndex = 0; pipelineIndex < entry.mIndex; pipelineIndex++)
					{
						if (entry.pPipelines.find(pipelineIndex) == entry.pPipelines.end())
							continue;

						const auto pPipeline = static_cast<VulkanGraphicsPipeline*>(entry.pPipelines.at(pipelineIndex).lock().get());
						pCommandBuffer->BindPipeline(pPipeline);

						const auto& drawResources = pPipeline->GetDrawResources();
						const UI64 drawResourceIndex = pPipeline->GetDrawResourcesIndex();

						for (UI64 drawIndex = 0; drawIndex < drawResourceIndex; drawIndex++)
						{
							if (drawResources.find(drawIndex) == drawResources.end())
								continue;

							const auto pResource = drawResources.at(drawIndex);
							pCommandBuffer->BindRenderResource(pResource);

							const auto& drawData = pResource->GetDrawData();
							const UI64 dIndex = pResource->GetDrawDataIndex();

							for (UI64 index = 0; index < dIndex; index++)
							{
								if (drawData.find(index) == drawData.end())
									continue;

								const auto& data = drawData.at(index);

								pCommandBuffer->SetDynamicStates(data.mDynamicStates);
								pCommandBuffer->DrawIndexed(data.mIndexCount, data.mIndexOffset, data.mVertexOffset);
							}
						}
					}
				}

				pCommandBuffer->EndRenderTarget();
				pCommandBuffer->EndBuffer();

				IncrementIndex();
			}
		}

		void VulkanScreenBoundRenderTarget::PrepareToDraw()
		{
			VulkanDevice* pvDevice = pDevice->GetAs<VulkanDevice>();
			FLINT_VK_ASSERT(vkWaitForFences(pvDevice->GetLogicalDevice(), 1, &vInFlightFences[mFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()), "Failed to wait for fence!");
			//FLINT_VK_ASSERT(pvDevice->ResetFences({ vInFlightFences[mFrameIndex] }), "Failed to reset fence!")

			VkResult result = vkAcquireNextImageKHR(pvDevice->GetLogicalDevice(), GetSwapChain(), std::numeric_limits<UI32>::max(), vImageAvailables[mFrameIndex], VK_NULL_HANDLE, &mImageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
				Recreate();
			else FLINT_VK_ASSERT(result, "Failed to acquire the next swap chain image!");

			if (mDynamicDrawEntries.size())
				SubmitSecondaryCommands();
		}

		void VulkanScreenBoundRenderTarget::Update()
		{
		}

		void VulkanScreenBoundRenderTarget::SubmitCommand()
		{
			VulkanDevice* pvDevice = pDevice->GetAs<VulkanDevice>();

			if (vImagesInFlightFences[mImageIndex] != VK_NULL_HANDLE)
				FLINT_VK_ASSERT(vkWaitForFences(pvDevice->GetLogicalDevice(), 1, &vImagesInFlightFences[mImageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()), "Failed to wait for fence!");

			vImagesInFlightFences[mImageIndex] = vInFlightFences[mFrameIndex];

			VkSemaphoreWaitFlags vWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkCommandBuffer vCommandBuffer[1] = { pCommandBufferList->GetAs<VulkanCommandBufferList>()->GetVkBuffer(mFrameIndex) };

			VkSubmitInfo vSI = {};
			vSI.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSI.pNext = VK_NULL_HANDLE;
			vSI.commandBufferCount = 1;
			vSI.pCommandBuffers = vCommandBuffer;
			vSI.waitSemaphoreCount = 1;
			vSI.pWaitSemaphores = &vImageAvailables[mFrameIndex];
			vSI.signalSemaphoreCount = 1;
			vSI.pSignalSemaphores = &vRenderFinishes[mFrameIndex];
			vSI.pWaitDstStageMask = &vWaitStage;

			FLINT_VK_ASSERT(vkResetFences(pvDevice->GetLogicalDevice(), 1, &vInFlightFences[mFrameIndex]), "Failed to reset fence!");
			FLINT_VK_ASSERT(pvDevice->SubmitQueue(pvDevice->GetGraphicsQueue(), { vSI }, vInFlightFences[mFrameIndex]), "Failed to submit queue!");

			VkPresentInfoKHR vPI = {};
			vPI.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			vPI.pNext = VK_NULL_HANDLE;
			vPI.waitSemaphoreCount = 1;
			vPI.pWaitSemaphores = &vRenderFinishes[mFrameIndex];
			vPI.swapchainCount = 1;

			VkSwapchainKHR vSwapChain = GetSwapChain();
			vPI.pSwapchains = &vSwapChain;
			vPI.pImageIndices = &mImageIndex;

			VkResult result = vkQueuePresentKHR(pvDevice->GetTransferQueue(), &vPI);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || pDisplay->GetInputCenter()->IsWindowResized)
			{
				pDisplay->GetInputCenter()->IsWindowResized = false;
				Recreate();
			}
			else FLINT_VK_ASSERT(result, "Failed to present queue!");

			FLINT_VK_ASSERT(vkQueueWaitIdle(pvDevice->GetTransferQueue()), "Failed to wait idle till the queue is complete!");

			IncrementIndex();
		}

		void VulkanScreenBoundRenderTarget::Recreate()
		{
			VulkanDevice* pvDevice = pDevice->GetAs<VulkanDevice>();
			pvDevice->WaitIdle();

			Vector2 extent = pDisplay->GetExtent();
			while (extent.width == 0.0f || extent.height == 0.0)
			{
				Thread::Utilities::SleepThisThread(std::chrono::milliseconds(1));

				pDisplay->Update();
				extent = pDisplay->GetExtent();
			}

			mExtent = extent;
			pCommandBufferList->ClearBuffers();

			// Destroy render pass and frame buffer.
			DestroyRenderPass(pvDevice);
			DestroyFrameBuffers(pvDevice);

			// Recreate attachments.
			vSwapChain.Recreate(extent);
			vColorBuffer.Recreate(extent);
			vDepthBuffer.Recreate(extent);

			// Recreate render pass and frame buffer.
			CreateRenderPass(pvDevice, { &vColorBuffer, &vDepthBuffer, &vSwapChain }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			CreateFrameBuffer(pvDevice, { &vColorBuffer, &vDepthBuffer, &vSwapChain }, mExtent, static_cast<UI32>(mBufferCount));

			// Re create renderable pipelines.
			for (auto itr = mStaticDrawEntries.begin(); itr != mStaticDrawEntries.end(); itr++)
				for (auto& pPipeline : itr->second.pPipelines)
					pPipeline.second.lock()->Recreate();

			for (auto itr = mDynamicDrawEntries.begin(); itr != mDynamicDrawEntries.end(); itr++)
				for (auto& pPipeline : itr->second.pPipelines)
					pPipeline.second.lock()->Recreate();

			// Prepare command buffers to be rendered.
			pCommandBufferList->ReceateBuffers();
			//for (auto& buffer : mChildCommandBuffers)
			//	buffer->Terminate();
			//
			//mChildCommandBuffers.clear();

			BakeCommands();

			vImagesInFlightFences.resize(vInFlightFences.size(), VK_NULL_HANDLE);

			mImageIndex = 0;
			mFrameIndex = 0;
		}

		void VulkanScreenBoundRenderTarget::SubmitSecondaryCommands()
		{
			//auto& pSecondaryCommandBuffers = mChildCommandBuffers[0];
			//mCommandBufferManager.UpdateChild(pSecondaryCommandBuffers.get(), this);
			//
			//auto& pCommandBuffer = mCommandBufferManager.GetBuffer(mImageIndex);
			//
			//std::vector<DrawEntry> dynamicVector;
			//for (auto itr = mDynamicDrawEntries.Begin(); itr != mDynamicDrawEntries.End(); itr++)
			//	INSERT_INTO_VECTOR(dynamicVector, itr->second);
			//
			//VkClearValue pClearValues[2] = {};
			//pClearValues[0].color.float32[0] = CREATE_COLOR_256(32.0f);
			//pClearValues[0].color.float32[1] = CREATE_COLOR_256(32.0f);
			//pClearValues[0].color.float32[2] = CREATE_COLOR_256(32.0f);
			//pClearValues[0].color.float32[3] = 1.0f;
			//pClearValues[1].depthStencil = { 1.0f, 0 };
			//
			//VkRenderPassBeginInfo vRBI = {};
			//vRBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			//vRBI.pNext = VK_NULL_HANDLE;
			//vRBI.clearValueCount = 2;
			//vRBI.pClearValues = pClearValues;
			//vRBI.renderPass = vRenderPass;
			//vRBI.framebuffer = vFrameBuffers[mImageIndex];
			//vRBI.renderArea.extent = { static_cast<UI32>(mExtent.x), static_cast<UI32>(mExtent.y) };
			//
			//pCommandBuffer->BeginBufferRecording();
			//auto pBuff = dynamic_cast<VulkanCommandBuffer*>(const_cast<CommandBuffer*>(pCommandBuffer.get()));
			//vkCmdBeginRenderPass(*pBuff, &vRBI, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
			//
			//BakeDynamicCommands(dynamicVector, pSecondaryCommandBuffers);
			//
			//pCommandBuffer->ExecuteChildCommands(pSecondaryCommandBuffers->GetBuffers());
			//
			//UnBind(pCommandBuffer);
			//pCommandBuffer->EndBufferRecording();
		}
	}
}

/*
{ 17:17:29 } Vulkan Validation Layer (Validation): Validation Error: [ VUID-vkQueueSubmit-pCommandBuffers-00072 ] Object 0: handle = 0x1e940001218, type = VK_OBJECT_TYPE_COMMAND_BUFFER; | MessageID = 0x772c8884 | VkCommandBuffer 0x1e940001218[] used in the call to vkQueueSubmit() is unrecorded and contains no commands. The Vulkan spec states: Any secondary command buffers recorded into any element of the pCommandBuffers member of any element of pSubmits must be in the pending or executable state (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkQueueSubmit-pCommandBuffers-00072)
{ 17:17:29 } Vulkan Validation Layer (Validation): Validation Error: [ VUID-VkPresentInfoKHR-pImageIndices-01296 ] Object 0: handle = 0x1e9405deb38, type = VK_OBJECT_TYPE_QUEUE; | MessageID = 0xc7aabc16 | vkQueuePresentKHR(): pSwapchains[0] images passed to present must be in layout VK_IMAGE_LAYOUT_PRESENT_SRC_KHR or VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR but is in VK_IMAGE_LAYOUT_UNDEFINED. The Vulkan spec states: Each element of pImageIndices must be the index of a presentable image acquired from the swapchain specified by the corresponding element of the pSwapchains array, and the presented image subresource must be in the VK_IMAGE_LAYOUT_PRESENT_SRC_KHR layout at the time the operation is executed on a VkDevice (https://github.com/KhronosGroup/Vulkan-Docs/search?q=)VUID-VkPresentInfoKHR-pImageIndices-01296)
*/