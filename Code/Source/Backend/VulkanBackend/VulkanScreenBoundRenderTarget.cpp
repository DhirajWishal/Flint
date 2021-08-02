// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanCommandBufferList.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "GeometryStore.hpp"

#include <iostream>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanScreenBoundRenderTarget::VulkanScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, UI32 threadCount)
			: ScreenBoundRenderTarget(pDevice, pDisplay, extent, bufferCount, pDevice->CreatePrimaryCommandBufferList(bufferCount), threadCount), vRenderTarget(pDevice->StaticCast<VulkanDevice>())
		{
			FLINT_SETUP_PROFILER();

			if (!mNumberOfThreads)
				pDefaultSecondaryCommandBuffer = std::make_unique<VulkanCommandBufferList>(pCommandBufferList->GetDevice(), 1, pCommandBufferList);

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			auto& vDisplay = pDisplay->StaticCast<VulkanDisplay>();

			pSwapChain = std::make_unique<VulkanSwapChain>(vDevice, vDisplay, extent, bufferCount);
			pColorBuffer = std::make_unique<VulkanColorBuffer>(vDevice, extent, bufferCount, pSwapChain->GetFormat());
			pDepthBuffer = std::make_unique<VulkanDepthBuffer>(vDevice, extent, bufferCount);

			vRenderTarget.CreateRenderPass({ pColorBuffer.get(), pDepthBuffer.get(), pSwapChain.get() }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			vRenderTarget.CreateFrameBuffer({ pColorBuffer.get(), pDepthBuffer.get(), pSwapChain.get() }, extent, bufferCount);
			vRenderTarget.CreateSyncObjects(bufferCount);

			// Setup default clear color values.
			pClearValues[0].color.float32[0] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[1] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[2] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[3] = 1.0f;
			pClearValues[1].depthStencil.depth = 1.0f;
			pClearValues[1].depthStencil.stencil = 0;

			// Issue the worker thread.
			AcquireAllThreads();

			vInheritInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

			vSI.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSI.pNext = VK_NULL_HANDLE;
			vSI.commandBufferCount = 1;
			vSI.waitSemaphoreCount = 1;
			vSI.signalSemaphoreCount = 1;

			vPI.sType = VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			vPI.pNext = VK_NULL_HANDLE;
			vPI.waitSemaphoreCount = 1;
			vPI.swapchainCount = 1;
		}

		void VulkanScreenBoundRenderTarget::PrepareStaticResources()
		{
			FLINT_SETUP_PROFILER();
			pThisRenderTarget = shared_from_this();
			BindSecondaryCommands();
		}

		void VulkanScreenBoundRenderTarget::BeginFrame()
		{
			FLINT_SETUP_PROFILER();

			// Wait while the window contains the right width and height. 
			// Until the window contains a valid size, it will skip draw calls.
			if (bShouldSkip)
			{
				Recreate();

				if (bShouldSkip)
					return;
			}
			else
				ExecuteSecondaryCommandBuffers();

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vkWaitForFences(vDevice.GetLogicalDevice(), 1, &vRenderTarget.vInFlightFences[mFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()));
			//FLINT_VK_ASSERT(pvDevice->ResetFences({ vInFlightFences[mFrameIndex] }), "Failed to reset fence!")

			VkResult result = vkAcquireNextImageKHR(vDevice.GetLogicalDevice(), pSwapChain->GetSwapChain(), std::numeric_limits<UI32>::max(), vRenderTarget.vImageAvailables[mFrameIndex], VK_NULL_HANDLE, &mImageIndex);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
				Recreate();
			else FLINT_VK_ASSERT(result);
		}

		void VulkanScreenBoundRenderTarget::Update()
		{
			FLINT_SETUP_PROFILER();
		}

		void VulkanScreenBoundRenderTarget::SubmitFrame()
		{
			FLINT_SETUP_PROFILER();

			auto& vDisplay = pDisplay->StaticCast<VulkanDisplay>();

			// Skip if the screen is reported to be 0 in width or height.
			if (bShouldSkip)
			{
				Recreate();
				return;
			}

			if (bIsAltered)
			{
				BindSecondaryCommands();
				ExecuteSecondaryCommandBuffers();

				bIsAltered = false;
			}

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			auto& vCommandBufferList = pCommandBufferList->StaticCast<VulkanCommandBufferList>();

			if (vRenderTarget.vImagesInFlightFences[mImageIndex] != VK_NULL_HANDLE)
				FLINT_VK_ASSERT(vkWaitForFences(vDevice.GetLogicalDevice(), 1, &vRenderTarget.vImagesInFlightFences[mImageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()));

			vRenderTarget.vImagesInFlightFences[mImageIndex] = vRenderTarget.vInFlightFences[mFrameIndex];

			vCommandBuffer[0] = vCommandBufferList.GetCommandBuffer(mFrameIndex);

			vSI.pCommandBuffers = vCommandBuffer;
			vSI.pWaitSemaphores = &vRenderTarget.vImageAvailables[mFrameIndex];
			vSI.pSignalSemaphores = &vRenderTarget.vRenderFinishes[mFrameIndex];
			vSI.pWaitDstStageMask = &vWaitStage;

			FLINT_VK_ASSERT(vkResetFences(vDevice.GetLogicalDevice(), 1, &vRenderTarget.vInFlightFences[mFrameIndex]));
			FLINT_VK_ASSERT(vkQueueSubmit(vDevice.GetQueue().vGraphicsQueue, 1, &vSI, vRenderTarget.vInFlightFences[mFrameIndex]));

			vPI.pWaitSemaphores = &vRenderTarget.vRenderFinishes[mFrameIndex];

			VkSwapchainKHR vSwapChain = pSwapChain->GetSwapChain();
			vPI.pSwapchains = &vSwapChain;
			vPI.pImageIndices = &mImageIndex;

			IncrementFrameIndex();
			VkResult result = vkQueuePresentKHR(vDevice.GetQueue().vTransferQueue, &vPI);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vDisplay.IsDisplayResized())
			{
				vDisplay.ToggleResize();
				Recreate();
			}
			else FLINT_VK_ASSERT(result);

			FLINT_VK_ASSERT(vkQueueWaitIdle(vDevice.GetQueue().vTransferQueue));

			// Bind all the secondary commands.
			BindSecondaryCommands();
		}

		void VulkanScreenBoundRenderTarget::Terminate()
		{
			vRenderTarget.Terminate();

			pSwapChain->Terminate();
			pColorBuffer->Terminate();
			pDepthBuffer->Terminate();

			pDevice->DestroyCommandBufferList(pCommandBufferList);

			TerminateThreads();

			if (pDefaultSecondaryCommandBuffer)
				pDevice->DestroyCommandBufferList(std::move(pDefaultSecondaryCommandBuffer));
		}

		void VulkanScreenBoundRenderTarget::Recreate()
		{
			FLINT_SETUP_PROFILER();

			FBox2D newExtent = pDisplay->GetExtent();
			if (newExtent.mWidth == 0 && newExtent.mHeight == 0)
			{
				bShouldSkip = true;
				return;
			}

			{
				std::lock_guard<std::mutex> guard(mResourceMutex);
				if (pCommandBufferList->IsRecording())
					ExecuteSecondaryCommandBuffers();
			}

			mExtent = newExtent;
			bShouldSkip = false;

			pDevice->WaitIdle();
			pCommandBufferList->ClearBuffers();

			vRenderTarget.DestroyRenderPass();
			vRenderTarget.DestroyFrameBuffers();

			pSwapChain->Recreate(mExtent);
			pColorBuffer->Recreate(mExtent);
			pDepthBuffer->Recreate(mExtent);

			vRenderTarget.CreateRenderPass({ pColorBuffer.get(), pDepthBuffer.get(), pSwapChain.get() }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			vRenderTarget.CreateFrameBuffer({ pColorBuffer.get(), pDepthBuffer.get(), pSwapChain.get() }, mExtent, mBufferCount);

			for (auto& instance : mDrawInstanceMaps)
			{
				for (auto& drawData : instance)
				{
					for (auto& pipeline : drawData.second)
					{
						pipeline->Recreate(pThisRenderTarget);
					}
				}
			}

			vRenderTarget.vInFlightFences.resize(vRenderTarget.vInFlightFences.size(), VK_NULL_HANDLE);
			mImageIndex = 0, mFrameIndex = 0;

			// Bake secondary commands.
			BindSecondaryCommands();
			ExecuteSecondaryCommandBuffers();
		}

		FColor4D VulkanScreenBoundRenderTarget::GetClearColor() const
		{
			return FColor4D(pClearValues[0].color.float32[0], pClearValues[0].color.float32[1], pClearValues[0].color.float32[2], pClearValues[0].color.float32[3]);
		}

		void VulkanScreenBoundRenderTarget::SetClearColor(const FColor4D& newColor)
		{
			pClearValues[0].color.float32[0] = newColor.mRed;
			pClearValues[0].color.float32[1] = newColor.mGreen;
			pClearValues[0].color.float32[2] = newColor.mBlue;
			pClearValues[0].color.float32[3] = newColor.mAlpha;
		}

		void VulkanScreenBoundRenderTarget::BindSecondaryCommands()
		{
			FLINT_SETUP_PROFILER();

			pCommandBufferList->BeginBufferRecording(mFrameIndex);
			pCommandBufferList->BindRenderTargetSecondary(pThisRenderTarget);

			vInheritInfo.renderPass = vRenderTarget.vRenderPass;
			vInheritInfo.framebuffer = GetFrameBuffer(mFrameIndex);

			if (mNumberOfThreads)
			{
				vInheritanceInfo.store(vInheritInfo);
				ReleaseAllThreads();
			}
			else
			{
				// Begin the command buffer.
				pDefaultSecondaryCommandBuffer->VulkanBeginSecondaryCommandBuffer(0, &vInheritInfo);

				// Bind the draw instances.
				for (auto& drawInstanceMap : mDrawInstanceMaps)
				{
					for (auto& instance : drawInstanceMap)
					{
						FLINT_SETUP_PROFILER();

						const auto geometryStore = instance.first;
						pDefaultSecondaryCommandBuffer->BindVertexBuffer(geometryStore->GetVertexBuffer());
						pDefaultSecondaryCommandBuffer->BindIndexBuffer(geometryStore->GetIndexBuffer(), geometryStore->GetIndexSize());

						// Iterate through the pipelines.
						for (auto& pipeline : instance.second)
						{
							pipeline->PrepareResourcesToDraw();
							pDefaultSecondaryCommandBuffer->BindGraphicsPipeline(pipeline);

							const auto drawData = pipeline->GetDrawData();

							// Bind draw data.
							for (const auto draw : drawData)
							{
								pDefaultSecondaryCommandBuffer->BindDrawResources(pipeline, draw.second.pResourceMap);
								pDefaultSecondaryCommandBuffer->BindDynamicStates(draw.second.pDynamicStates);
								pDefaultSecondaryCommandBuffer->IssueDrawCall(draw.second.mVertexOffset, draw.second.mVertexCount, draw.second.mIndexOffset, draw.second.mIndexCount);
							}
						}
					}
				}

				// End buffer recording and submit it to execute.
				pDefaultSecondaryCommandBuffer->EndBufferRecording();
				vSecondaryCommandBuffers.push_back(pDefaultSecondaryCommandBuffer->GetCurrentCommandBuffer());
			}
		}

		void VulkanScreenBoundRenderTarget::SecondaryCommandsWorker(DrawInstanceMap& drawInstanceMap, BinarySemaphore& binarySemaphore, CountingSemaphore& countingSemaphore, std::atomic<bool>& shouldRun)
		{
			mResourceMutex.lock();
			VulkanCommandBufferList vCommandBufferList{ pCommandBufferList->GetDevice(), 1, pCommandBufferList };
			mResourceMutex.unlock();

			countingSemaphore.Release();

			do
			{
				if (binarySemaphore.TryAcquire())
				{
					// Begin the command buffer.
					VkCommandBufferInheritanceInfo vInheritInfo = vInheritanceInfo.load();
					vCommandBufferList.VulkanBeginSecondaryCommandBuffer(0, &vInheritInfo);

					// Bind the draw instances.
					for (auto& instance : drawInstanceMap)
					{
						FLINT_SETUP_PROFILER();

						const auto geometryStore = instance.first;
						vCommandBufferList.BindVertexBuffer(geometryStore->GetVertexBuffer());
						vCommandBufferList.BindIndexBuffer(geometryStore->GetIndexBuffer(), geometryStore->GetIndexSize());

						// Iterate through the pipelines.
						for (auto& pipeline : instance.second)
						{
							pipeline->PrepareResourcesToDraw();
							vCommandBufferList.BindGraphicsPipeline(pipeline);

							const auto drawData = pipeline->GetDrawData();

							// Bind draw data.
							for (const auto draw : drawData)
							{
								vCommandBufferList.BindDrawResources(pipeline, draw.second.pResourceMap);
								vCommandBufferList.BindDynamicStates(draw.second.pDynamicStates);
								vCommandBufferList.IssueDrawCall(draw.second.mVertexOffset, draw.second.mVertexCount, draw.second.mIndexOffset, draw.second.mIndexCount);
							}
						}
					}

					// End buffer recording and submit it to execute.
					vCommandBufferList.EndBufferRecording();

					{
						std::lock_guard<std::mutex> guard(mResourceMutex);
						vSecondaryCommandBuffers.push_back(vCommandBufferList.GetCurrentCommandBuffer());
					}

					countingSemaphore.Release();
				}
			} while (shouldRun);

			vCommandBufferList.Terminate();
		}

		void VulkanScreenBoundRenderTarget::ExecuteSecondaryCommandBuffers()
		{
			AcquireAllThreads();

			pCommandBufferList->StaticCast<VulkanCommandBufferList>().SetSecondaryCommandBuffers(std::move(vSecondaryCommandBuffers));
			pCommandBufferList->ExecuteSecondaryCommands();
			pCommandBufferList->UnbindRenderTarget();
			pCommandBufferList->EndBufferRecording();
		}
	}
}