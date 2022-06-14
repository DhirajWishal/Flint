// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanWindow.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanRenderTargetAttachment.hpp"

#include <array>

#include <SDL_vulkan.h>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanWindow::VulkanWindow(VulkanDevice& device, std::string&& title, uint32_t width /*= -1*/, uint32_t height /*= -1*/)
			: Window(device, std::move(title), width, height)
		{
			// Resolve the flags.
			uint32_t windowFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;

			if (getWidth() == 0 || getHeight() == 0)
				windowFlags |= SDL_WINDOW_MAXIMIZED;
			else if (getWidth() == -1 || getHeight() == -1)
				windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

			// Create the window.
			m_pWindow = SDL_CreateWindow(getTitle().data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, getWidth(), getHeight(), windowFlags);

			// Set this class as user data.
			SDL_SetWindowData(m_pWindow, "this", this);

			// Check if we were able to create the window.
			if (!m_pWindow)
				throw BackendError("Failed to create the window!");

			// Refresh the size.
			refreshExtent();

			// Make sure to show the window.
			SDL_ShowWindow(m_pWindow);

			// Create the surface.
			createSurface();

			// Get the best buffer count.
			m_FrameCount = getBestBufferCount();
			notifyUpdated();

			// Create the command buffer.
			m_pCommandBuffers = std::make_unique<VulkanCommandBuffers>(device, m_FrameCount);

			// Create the swapchain.
			createSwapchain();

			// Create the render pass.
			createRenderPass();

			// Create frame buffers.
			createFramebuffers();

			// Create the semaphores.
			createSyncObjects();
		}

		VulkanWindow::~VulkanWindow()
		{
			// Wait till we finish whatever we are running.
			getDevice().waitIdle();

			// Destroy the semaphores.
			destroySyncObjects();

			// Destroy the render pass.
			destroyRenderPass();

			// Destroy the frame buffers.
			destroyFramebuffers();

			// Clear the swapchain.
			clearSwapchain();

			// Destroy the surface.
			destroySurface();

			// Destroy the SDL window.
			SDL_DestroyWindow(m_pWindow);
		}

		void VulkanWindow::update()
		{
			// If we're minimized, skip.
			if (SDL_GetWindowFlags(m_pWindow) & SDL_WINDOW_MINIMIZED)
				return;

			// We need to skip things if we're running this function for the first time.
			if (!m_FirstTime)
			{
				copyAndSubmitFrame();
				present();
			}

			// If we're minimized, skip. This is needed again because we would potentially recreate things from the inside.
			if (SDL_GetWindowFlags(m_pWindow) & SDL_WINDOW_MINIMIZED)
				return;

			// Acquire the next swapchain image.
			const auto result = getDevice().getDeviceTable().vkAcquireNextImageKHR(getDevice().getLogicalDevice(), m_Swapchain, std::numeric_limits<uint64_t>::max(), m_InFlightSemaphores[m_FrameIndex], VK_NULL_HANDLE, &m_ImageIndex);
			if (result == VkResult::VK_ERROR_OUT_OF_DATE_KHR || result == VkResult::VK_SUBOPTIMAL_KHR)
			{
				recreate();
				update();
			}

			FLINT_VK_ASSERT(result, "Failed to acquire the next swap chain image!");

			m_FirstTime = false;
		}

		uint32_t VulkanWindow::getBestBufferCount() const
		{
			// Get the surface capabilities.
			VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
			FLINT_VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getDevice().getPhysicalDevice(), m_Surface, &surfaceCapabilities), "Failed to get the surface capabilities!");

			// Resolve the best buffer count.
			uint32_t bufferCount = surfaceCapabilities.minImageCount + 1;
			if (surfaceCapabilities.maxImageCount > 0 && bufferCount > surfaceCapabilities.maxImageCount)
				bufferCount = surfaceCapabilities.maxImageCount;

			return bufferCount;
		}

		void VulkanWindow::refreshExtent()
		{
			int32_t width = 0, height = 0;
			SDL_Vulkan_GetDrawableSize(m_pWindow, &width, &height);

			m_Width = width;
			m_Height = height;
		}

		void VulkanWindow::createSurface()
		{
			if (SDL_Vulkan_CreateSurface(m_pWindow, getDevice().getInstance().getInstance(), &m_Surface) == SDL_FALSE)
				throw BackendError("Failed to create the window surface!");
		}

		void VulkanWindow::destroySurface()
		{
			vkDestroySurfaceKHR(getDevice().getInstance().getInstance(), m_Surface, nullptr);
		}

		void VulkanWindow::clearSwapchain()
		{
			// Terminate the image views.
			for (auto view : m_SwapchainImageViews)
				getDevice().getDeviceTable().vkDestroyImageView(getDevice().getLogicalDevice(), view, nullptr);

			// Now we can destroy the swapchain.
			getDevice().getDeviceTable().vkDestroySwapchainKHR(getDevice().getLogicalDevice(), m_Swapchain, nullptr);

			// Clean the variables.
			m_Swapchain = VK_NULL_HANDLE;
			m_SwapchainImages.clear();
			m_SwapchainImageViews.clear();
		}

		void VulkanWindow::resolveImageViews()
		{
			VkImageViewCreateInfo viewCreateInfo = {};
			viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewCreateInfo.pNext = VK_NULL_HANDLE;
			viewCreateInfo.flags = 0;
			viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewCreateInfo.format = m_SwapchainFormat;
			viewCreateInfo.components = {};
			viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewCreateInfo.subresourceRange.baseMipLevel = 0;
			viewCreateInfo.subresourceRange.levelCount = 1;
			viewCreateInfo.subresourceRange.baseArrayLayer = 0;
			viewCreateInfo.subresourceRange.layerCount = 1;

			m_SwapchainImageViews.resize(m_SwapchainImages.size());

			// Iterate over the image views and create them.
			VkImageView* pArray = m_SwapchainImageViews.data();
			for (auto itr = m_SwapchainImages.begin(); itr != m_SwapchainImages.end(); ++itr, ++pArray)
			{
				viewCreateInfo.image = *itr;
				FLINT_VK_ASSERT(getDevice().getDeviceTable().vkCreateImageView(getDevice().getLogicalDevice(), &viewCreateInfo, nullptr, pArray), "Failed to create the swapchain image view!");
			}
		}

		void VulkanWindow::createSwapchain()
		{
			// Get the surface capabilities.
			VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
			FLINT_VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getDevice().getPhysicalDevice(), m_Surface, &surfaceCapabilities), "Failed to get the surface capabilities!");

			// Get the surface formats.
			uint32_t formatCount = 0;
			FLINT_VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(getDevice().getPhysicalDevice(), m_Surface, &formatCount, nullptr), "Failed to get the surface format count!");

			if (formatCount == 0)
				throw BackendError("No suitable surface formats found!");

			std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
			FLINT_VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(getDevice().getPhysicalDevice(), m_Surface, &formatCount, surfaceFormats.data()), "Failed to get the surface formats!");

			// Get the present modes.
			uint32_t presentModeCount = 0;
			FLINT_VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(getDevice().getPhysicalDevice(), m_Surface, &presentModeCount, nullptr), "Failed to get the surface present mode count!");

			if (presentModeCount == 0)
				throw BackendError("No suitable present formats found!");

			std::vector<VkPresentModeKHR> presentModes(presentModeCount);
			FLINT_VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(getDevice().getPhysicalDevice(), m_Surface, &presentModeCount, presentModes.data()), "Failed to get the surface present modes!");

			// Check if we have the present mode we need.
			bool bPresentModeAvailable = false;
			VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			for (const auto availablePresentMode : presentModes)
			{
				if (availablePresentMode == presentMode)
				{
					bPresentModeAvailable = true;
					break;
				}
			}

			// If not available, let's just use the first one we got.
			if (!bPresentModeAvailable)
				presentMode = presentModes.front();

			// Resolve the surface composite.
			VkCompositeAlphaFlagBitsKHR surfaceComposite = static_cast<VkCompositeAlphaFlagBitsKHR>(surfaceCapabilities.supportedCompositeAlpha);
			surfaceComposite = (surfaceComposite & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
				? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
				: VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

			// Get the best surface format.
			VkSurfaceFormatKHR surfaceFormat = surfaceFormats.front();
			for (const auto& availableFormat : surfaceFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					surfaceFormat = availableFormat;
			}

			m_SwapchainFormat = surfaceFormat.format;

			// Create the swap chain.
			VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
			swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainCreateInfo.pNext = VK_NULL_HANDLE;
			swapchainCreateInfo.flags = 0;
			swapchainCreateInfo.surface = m_Surface;
			swapchainCreateInfo.minImageCount = m_FrameCount;
			swapchainCreateInfo.imageFormat = m_SwapchainFormat;
			swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
			swapchainCreateInfo.imageExtent.width = m_Width;
			swapchainCreateInfo.imageExtent.height = m_Height;
			swapchainCreateInfo.imageArrayLayers = 1;
			swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = nullptr;
			swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
			swapchainCreateInfo.compositeAlpha = surfaceComposite;
			swapchainCreateInfo.presentMode = presentMode;
			swapchainCreateInfo.clipped = VK_TRUE;
			swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

			// Resolve the queue families if the two queues are different.
			uint32_t queueFamilyindices[2] = {
				getDevice().getGraphicsQueue().m_Family,
				getDevice().getTransferQueue().m_Family
			};

			if (queueFamilyindices[0] != queueFamilyindices[1])
			{
				swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				swapchainCreateInfo.queueFamilyIndexCount = 2;
				swapchainCreateInfo.pQueueFamilyIndices = queueFamilyindices;
			}

			FLINT_VK_ASSERT(getDevice().getDeviceTable().vkCreateSwapchainKHR(getDevice().getLogicalDevice(), &swapchainCreateInfo, nullptr, &m_Swapchain), "Failed to create the swapchain!");

			// Get the image views.
			m_SwapchainImages.resize(swapchainCreateInfo.minImageCount);
			FLINT_VK_ASSERT(getDevice().getDeviceTable().vkGetSwapchainImagesKHR(getDevice().getLogicalDevice(), m_Swapchain, &swapchainCreateInfo.minImageCount, m_SwapchainImages.data()), "Failed to get the swapchain images!");

			// Finally we can resolve the swapchain image views.
			resolveImageViews();
		}

		void VulkanWindow::createSyncObjects()
		{
			VkSemaphoreCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;

			// Reserve to allocate everything once.
			m_RenderFinishedSemaphores.reserve(m_FrameCount);
			m_InFlightSemaphores.reserve(m_FrameCount);

			// Iterate over and create the semaphores.
			for (uint8_t i = 0; i < m_FrameCount; i++)
			{
				FLINT_VK_ASSERT(getDevice().getDeviceTable().vkCreateSemaphore(getDevice().getLogicalDevice(), &createInfo, nullptr, &m_RenderFinishedSemaphores.emplace_back()), "Failed to create the frame buffer!");
				FLINT_VK_ASSERT(getDevice().getDeviceTable().vkCreateSemaphore(getDevice().getLogicalDevice(), &createInfo, nullptr, &m_InFlightSemaphores.emplace_back()), "Failed to create the frame buffer!");
			}
		}

		void VulkanWindow::destroySyncObjects()
		{
			for (uint32_t i = 0; i < m_FrameCount; i++)
			{
				getDevice().getDeviceTable().vkDestroySemaphore(getDevice().getLogicalDevice(), m_RenderFinishedSemaphores[i], nullptr);
				getDevice().getDeviceTable().vkDestroySemaphore(getDevice().getLogicalDevice(), m_InFlightSemaphores[i], nullptr);
			}
		}

		void VulkanWindow::createRenderPass()
		{
			// Crate attachment descriptions.
			VkAttachmentDescription attachmentDescription = {};
			attachmentDescription.flags = 0;
			attachmentDescription.format = m_SwapchainFormat;
			attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
			attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;	// TODO: Play around with this so that we don't do anything stupid with the depencency copy.
			attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			// Create the subpass dependencies.
			std::array<VkSubpassDependency, 2> subpassDependencies;
			subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencies[0].dstSubpass = 0;
			subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			subpassDependencies[1].srcSubpass = 0;
			subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			// Create the subpass description.
			VkAttachmentReference colorAttachmentReference = {};
			colorAttachmentReference.attachment = 0;
			colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpassDescription = {};
			subpassDescription.flags = 0;
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.inputAttachmentCount = 0;
			subpassDescription.pInputAttachments = nullptr;
			subpassDescription.colorAttachmentCount = 1;
			subpassDescription.pColorAttachments = &colorAttachmentReference;
			subpassDescription.pResolveAttachments = nullptr;
			subpassDescription.pDepthStencilAttachment = nullptr;
			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = nullptr;

			// Create the render target.
			VkRenderPassCreateInfo renderPassCreateInfo = {};
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.pNext = nullptr;
			renderPassCreateInfo.flags = 0;
			renderPassCreateInfo.attachmentCount = 1;
			renderPassCreateInfo.pAttachments = &attachmentDescription;
			renderPassCreateInfo.subpassCount = 1;
			renderPassCreateInfo.pSubpasses = &subpassDescription;
			renderPassCreateInfo.dependencyCount = 2;
			renderPassCreateInfo.pDependencies = subpassDependencies.data();

			FLINT_VK_ASSERT(getDevice().getDeviceTable().vkCreateRenderPass(getDevice().getLogicalDevice(), &renderPassCreateInfo, nullptr, &m_RenderPass), "Failed to create render pass!");
		}

		void VulkanWindow::destroyRenderPass()
		{
			getDevice().getDeviceTable().vkDestroyRenderPass(getDevice().getLogicalDevice(), m_RenderPass, nullptr);
		}

		void VulkanWindow::createFramebuffers()
		{
			VkFramebufferCreateInfo frameBufferCreateInfo = {};
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.pNext = VK_NULL_HANDLE;
			frameBufferCreateInfo.flags = 0;
			frameBufferCreateInfo.renderPass = m_RenderPass;
			frameBufferCreateInfo.attachmentCount = 1;
			frameBufferCreateInfo.width = m_Width;
			frameBufferCreateInfo.height = m_Height;
			frameBufferCreateInfo.layers = 1;

			// Iterate and create the frame buffers.
			m_Framebuffers.resize(m_FrameCount);
			for (uint8_t i = 0; i < m_FrameCount; i++)
			{
				frameBufferCreateInfo.pAttachments = &m_SwapchainImageViews[i];
				FLINT_VK_ASSERT(getDevice().getDeviceTable().vkCreateFramebuffer(getDevice().getLogicalDevice(), &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]), "Failed to create the frame buffer!");
			}
		}

		void VulkanWindow::destroyFramebuffers()
		{
			for (const auto framebuffer : m_Framebuffers)
				getDevice().getDeviceTable().vkDestroyFramebuffer(getDevice().getLogicalDevice(), framebuffer, nullptr);
		}

		void VulkanWindow::recreate()
		{
			// Before we do anything, check if we're minimized.
			if (SDL_GetWindowFlags(m_pWindow) & SDL_WINDOW_MINIMIZED)
			{
				m_FirstTime = true;
				return;
			}

			// Wait till we finish whatever we are running.
			getDevice().waitIdle();

			// Get the new extent.
			refreshExtent();

			// Destroy the previous stuff.
			getDevice().getDeviceTable().vkDestroyRenderPass(getDevice().getLogicalDevice(), m_RenderPass, nullptr);

			for (uint32_t i = 0; i < m_FrameCount; i++)
			{
				getDevice().getDeviceTable().vkDestroyFramebuffer(getDevice().getLogicalDevice(), m_Framebuffers[i], nullptr);
				getDevice().getDeviceTable().vkDestroySemaphore(getDevice().getLogicalDevice(), m_RenderFinishedSemaphores[i], nullptr);
				getDevice().getDeviceTable().vkDestroySemaphore(getDevice().getLogicalDevice(), m_InFlightSemaphores[i], nullptr);
			}

			m_RenderFinishedSemaphores.clear();
			m_InFlightSemaphores.clear();

			// Make sure to destroy the old surface!
			clearSwapchain();
			vkDestroySurfaceKHR(getDevice().getInstance().getInstance(), m_Surface, nullptr);

			// Now we can redo it.
			createSurface();
			createSwapchain();
			createRenderPass();
			createFramebuffers();
			createSyncObjects();

			// Reset the indexes and the state.
			m_FrameIndex = 0;
			m_ImageIndex = 0;
			m_FirstTime = true;
			m_pCommandBuffers->resetIndex();
			toggleNeedToUpdate();
		}

		void VulkanWindow::copyAndSubmitFrame()
		{
			// Begin the command buffer recording.
			m_pCommandBuffers->finishExecution();

			// Update ONLY if we have anything to update.
			if (needToUpdate())
			{
				m_pCommandBuffers->begin();

				// If we have a dependency, let's copy it.
				if (m_Dependency.first)
				{
					auto pAttachment = m_Dependency.first->getAttachment(m_Dependency.second).as<VulkanRenderTargetAttachment>();
					const auto currentSwapchainImage = m_SwapchainImages[m_FrameIndex];

					VkImageSubresourceLayers subresourceLayers = {};
					subresourceLayers.baseArrayLayer = 0;
					subresourceLayers.layerCount = 1;
					subresourceLayers.mipLevel = 0;
					subresourceLayers.aspectMask = pAttachment->getType() == Core::AttachmentType::Color ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT;

					// Prepare to transfer.
					m_pCommandBuffers->changeImageLayout(pAttachment->getImage(), pAttachment->getLayout(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, subresourceLayers.aspectMask);
					m_pCommandBuffers->changeImageLayout(currentSwapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

					// If the attachment's size is not equal to the size of the window, we need to either upscale or downscale the image.
					if (pAttachment->getWidth() != getWidth() || pAttachment->getHeight() != getHeight())
					{
						// Later we can use an upscaling technology like DLSS or FidelityFX.

						VkImageBlit imageBlit = {};
						imageBlit.srcOffsets[0].z = imageBlit.srcOffsets[0].y = imageBlit.srcOffsets[0].x = 0;
						imageBlit.srcOffsets[1].x = static_cast<int32_t>(pAttachment->getWidth());
						imageBlit.srcOffsets[1].y = static_cast<int32_t>(pAttachment->getHeight());
						imageBlit.srcOffsets[1].z = 1;
						imageBlit.srcSubresource = subresourceLayers;
						imageBlit.dstOffsets[0].z = imageBlit.dstOffsets[0].y = imageBlit.dstOffsets[0].x = 0;
						imageBlit.dstOffsets[1].x = static_cast<int32_t>(getWidth());
						imageBlit.dstOffsets[1].y = static_cast<int32_t>(getHeight());
						imageBlit.dstOffsets[1].z = 1;
						imageBlit.dstSubresource = subresourceLayers;

						// Copy the image.
						getDevice().getDeviceTable().vkCmdBlitImage(m_pCommandBuffers->getCurrentBuffer(), pAttachment->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, currentSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);
					}

					// Else we can just perform a copy.
					else
					{
						VkImageCopy imageCopy = {};
						imageCopy.srcOffset.z = imageCopy.srcOffset.y = imageCopy.srcOffset.x = 0;
						imageCopy.srcSubresource = subresourceLayers;
						imageCopy.dstOffset.z = imageCopy.dstOffset.y = imageCopy.dstOffset.x = 0;
						imageCopy.dstSubresource = subresourceLayers;
						imageCopy.extent.width = pAttachment->getWidth();
						imageCopy.extent.height = pAttachment->getHeight();
						imageCopy.extent.depth = 1;

						// Copy the image.
						getDevice().getDeviceTable().vkCmdCopyImage(m_pCommandBuffers->getCurrentBuffer(), pAttachment->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, currentSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
					}

					// Change back to previous.
					m_pCommandBuffers->changeImageLayout(pAttachment->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pAttachment->getLayout(), subresourceLayers.aspectMask);
					m_pCommandBuffers->changeImageLayout(currentSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);
				}

				// Bind the window.
				VkClearValue clearValue = {};
				clearValue.color.float32[0] = 0.0f;
				clearValue.color.float32[1] = 0.0f;
				clearValue.color.float32[2] = 0.0f;
				clearValue.color.float32[3] = 1.0f;

				m_pCommandBuffers->bindWindow(*this, clearValue);

				// Unbind the window.
				m_pCommandBuffers->unbindWindow();

				// End the command buffer recording.
				m_pCommandBuffers->end();

				// Reduce the variable.
				notifyUpdated();
			}

			// Submit the commands to the GPU.
			m_pCommandBuffers->submit(m_RenderFinishedSemaphores[m_FrameIndex], m_InFlightSemaphores[m_FrameIndex]);

			// Iterate to the next command buffer.
			m_pCommandBuffers->next();

			// Increment to the next index.
			m_FrameIndex = ++m_FrameIndex % m_FrameCount;
		}

		void VulkanWindow::present()
		{
			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pNext = nullptr;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_ImageIndex];
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &m_Swapchain;
			presentInfo.pImageIndices = &m_ImageIndex;
			presentInfo.pResults = VK_NULL_HANDLE;

			// Present it to the surface.
			const auto result = getDevice().getDeviceTable().vkQueuePresentKHR(getDevice().getTransferQueue().m_Queue, &presentInfo);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
				recreate();

			else
				FLINT_VK_ASSERT(result, "Failed to present the swapchain image!");
		}
	}
}