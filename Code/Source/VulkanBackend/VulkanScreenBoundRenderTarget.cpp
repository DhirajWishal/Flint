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
			pColorBuffer = std::make_unique<VulkanColorBuffer>(vDevice, extent, bufferCount, pSwapChain->GetImageFormat());
			pDepthBuffer = std::make_unique<VulkanDepthBuffer>(vDevice, extent, bufferCount);

			std::vector<VkSubpassDependency> vDependencies{ 2 };

			vDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[0].dstSubpass = 0;
			vDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			vDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			vDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			vDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			vDependencies[1].srcSubpass = 0;
			vDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			vDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			vDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			vDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			vRenderTarget.CreateRenderPass({ pColorBuffer.get(), pDepthBuffer.get(), pSwapChain.get() }, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
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

			if (bShouldSkip)
			{
				Recreate();

				if (bShouldSkip)
					return;
			}
			else
				AcquireAllThreads();
			//ExecuteSecondaryCommandBuffers();

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vkWaitForFences(vDevice.GetLogicalDevice(), 1, &vRenderTarget.vInFlightFences[mFrameIndex], VK_TRUE, UI64_MAX));

			VkResult result = vkAcquireNextImageKHR(vDevice.GetLogicalDevice(), pSwapChain->GetSwapChain(), UI64_MAX, vRenderTarget.vImageAvailables[mFrameIndex], VK_NULL_HANDLE, &mImageIndex);
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
				return;
			else if (bIsAltered)
			{
				//AcquireAllThreads();
				//BindVolatileInstances();
				ExecuteSecondaryCommandBuffers();

				BindSecondaryCommands();
				AcquireAllThreads();

				// Bind all the volatile instances.
				BindVolatileInstances();
				ExecuteSecondaryCommandBuffers();

				bIsAltered = false;
			}
			else
			{
				AcquireAllThreads();

				// Bind all the volatile instances.
				BindVolatileInstances();
				ExecuteSecondaryCommandBuffers();
			}

			vCommandBuffer[0] = pCommandBufferList->StaticCast<VulkanCommandBufferList>().GetCommandBuffer(mFrameIndex);

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			if (vRenderTarget.vImagesInFlightFences[mImageIndex] != VK_NULL_HANDLE)
				FLINT_VK_ASSERT(vkWaitForFences(vDevice.GetLogicalDevice(), 1, &vRenderTarget.vImagesInFlightFences[mImageIndex], VK_TRUE, UI64_MAX));

			vRenderTarget.vImagesInFlightFences[mImageIndex] = vRenderTarget.vInFlightFences[mFrameIndex];

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

			// Bind all the secondary commands.
			BindSecondaryCommands();

			FLINT_VK_ASSERT(vkQueueWaitIdle(vDevice.GetQueue().vTransferQueue));
		}

		void VulkanScreenBoundRenderTarget::Terminate()
		{
			vRenderTarget.Terminate();

			pSwapChain->Terminate();
			pColorBuffer->Terminate();
			pDepthBuffer->Terminate();

			pDevice->DestroyCommandBufferList(pCommandBufferList);
			pDevice->DestroyCommandBufferList(pVolatileCommandBufferList);

			TerminateThreads();

			if (pDefaultSecondaryCommandBuffer)
				pDevice->DestroyCommandBufferList(std::move(pDefaultSecondaryCommandBuffer));
		}

		void VulkanScreenBoundRenderTarget::Recreate()
		{
			FLINT_SETUP_PROFILER();

			FBox2D newExtent = pDisplay->GetExtent();
			if (newExtent.IsZero())
			{
				bShouldSkip = true;
				return;
			}

			{
				std::lock_guard<std::mutex> guard(mResourceMutex);
				if (pCommandBufferList->IsRecording())
				{
					AcquireAllThreads();
					ExecuteSecondaryCommandBuffers();
				}
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

			std::vector<VkSubpassDependency> vDependencies{ 2 };

			vDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[0].dstSubpass = 0;
			vDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			vDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			vDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			vDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			vDependencies[1].srcSubpass = 0;
			vDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			vDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			vDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			vDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			vRenderTarget.CreateRenderPass({ pColorBuffer.get(), pDepthBuffer.get(), pSwapChain.get() }, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
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
			AcquireAllThreads();

			BindVolatileInstances();
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

		void VulkanScreenBoundRenderTarget::BindVolatileInstances()
		{
			// Begin the command buffer.
			auto& vCommandBufferList = pVolatileCommandBufferList->StaticCast<VulkanCommandBufferList>();
			vCommandBufferList.VulkanBeginSecondaryCommandBuffer(0, &vInheritInfo);

			// Bind the draw instances.
			for (const auto store : mVolatileDrawInstanceOrder)
			{
				FLINT_SETUP_PROFILER();

				if (!store->GetVertexBuffer() || !store->GetIndexBuffer())
					continue;

				vCommandBufferList.BindVertexBuffer(store->GetVertexBuffer());
				vCommandBufferList.BindIndexBuffer(store->GetIndexBuffer(), store->GetIndexSize());

				auto& pipelines = mVolatileDrawInstanceMap.at(store);

				// Iterate through the pipelines.
				for (auto& pipeline : pipelines)
				{
					pipeline->PrepareResourcesToDraw();
					vCommandBufferList.BindGraphicsPipeline(pipeline);

					const auto drawData = pipeline->GetDrawData();
					const auto drawDataIndex = pipeline->GetCurrentDrawIndex();

					// Bind draw data.
					for (UI64 i = 0; i < drawDataIndex; i++)
					{
						if (drawData.find(i) == drawData.end())
							continue;

						const auto draw = drawData.at(i);
						vCommandBufferList.BindDrawResources(pipeline, draw.pResourceMap);
						vCommandBufferList.BindDynamicStates(pipeline, draw.pDynamicStates);
						vCommandBufferList.IssueDrawCall(draw.mVertexOffset, draw.mVertexCount, draw.mIndexOffset, draw.mIndexCount);
					}
				}
			}

			// End buffer recording and submit it to execute.
			vCommandBufferList.EndBufferRecording();
			vSecondaryCommandBuffers.push_back(vCommandBufferList.GetCurrentCommandBuffer());
		}

		void VulkanScreenBoundRenderTarget::BindSecondaryCommands()
		{
			FLINT_SETUP_PROFILER();

			pCommandBufferList->BeginBufferRecording(mFrameIndex);

			// Execute the render targets. TODO
			for (auto pOffScreenRenderTarget : pOffScreenRenderTargets)
				pOffScreenRenderTarget->Execute(pThisRenderTarget);

			// Execute the off screen render target content.
			//pCommandBufferList->ExecuteSecondaryCommands();

			// Bind this render target for the secondary command buffers.
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
				for (UI64 itr = 0; itr < mDrawInstanceOrder.size(); itr++)
				{
					const auto drawOrder = mDrawInstanceOrder[itr];
					const auto drawInstanceMap = mDrawInstanceMaps[itr];

					for (const auto store : drawOrder)
					{
						FLINT_SETUP_PROFILER();

						pDefaultSecondaryCommandBuffer->BindVertexBuffer(store->GetVertexBuffer());
						pDefaultSecondaryCommandBuffer->BindIndexBuffer(store->GetIndexBuffer(), store->GetIndexSize());

						auto& pipelines = drawInstanceMap.at(store);

						// Iterate through the pipelines.
						for (auto& pipeline : pipelines)
						{
							pipeline->PrepareResourcesToDraw();
							pDefaultSecondaryCommandBuffer->BindGraphicsPipeline(pipeline);

							const auto drawData = pipeline->GetDrawData();

							// Bind draw data.
							for (const auto draw : drawData)
							{
								pDefaultSecondaryCommandBuffer->BindDrawResources(pipeline, draw.second.pResourceMap);
								pDefaultSecondaryCommandBuffer->BindDynamicStates(pipeline, draw.second.pDynamicStates);
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

		void VulkanScreenBoundRenderTarget::SecondaryCommandsWorker(DrawInstanceMap& drawInstanceMap, std::list<std::shared_ptr<GeometryStore>>& drawOrder, BinarySemaphore& binarySemaphore, CountingSemaphore& countingSemaphore, std::atomic<bool>& shouldRun)
		{
			mResourceMutex.lock();
			VulkanCommandBufferList vCommandBufferList{ pCommandBufferList->GetDevice(), pCommandBufferList->GetBufferCount(), pCommandBufferList };
			mResourceMutex.unlock();

			countingSemaphore.Release();

			do
			{
				if (binarySemaphore.TryAcquire())
				{
					// Begin the command buffer.
					VkCommandBufferInheritanceInfo vInheritInfo = vInheritanceInfo.load();
					vCommandBufferList.VulkanBeginNextSecondaryCommandBuffer(&vInheritInfo);

					// Bind the draw instances.
					for (const auto store : drawOrder)
					{
						FLINT_SETUP_PROFILER();

						vCommandBufferList.BindVertexBuffer(store->GetVertexBuffer());
						vCommandBufferList.BindIndexBuffer(store->GetIndexBuffer(), store->GetIndexSize());

						auto& pipelines = drawInstanceMap[store];

						// Iterate through the pipelines.
						for (auto& pipeline : pipelines)
						{
							pipeline->PrepareResourcesToDraw();
							vCommandBufferList.BindGraphicsPipeline(pipeline);

							const auto drawData = pipeline->GetDrawData();

							// Bind draw data.
							for (const auto draw : drawData)
							{
								vCommandBufferList.BindDrawResources(pipeline, draw.second.pResourceMap);
								vCommandBufferList.BindDynamicStates(pipeline, draw.second.pDynamicStates);
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
			if (!pCommandBufferList->IsRecording())
				return;

			pCommandBufferList->StaticCast<VulkanCommandBufferList>().SetSecondaryCommandBuffers(std::move(vSecondaryCommandBuffers));
			pCommandBufferList->ExecuteSecondaryCommands();
			pCommandBufferList->UnbindRenderTarget();
			pCommandBufferList->EndBufferRecording();
		}
	}
}