// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBuffers.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanWindow.hpp"
#include "VulkanBackend/VulkanRasterizer.hpp"
#include "VulkanBackend/VulkanRasterizingPipeline.hpp"
#include "VulkanBackend/VulkanRasterizingProgram.hpp"
#include "VulkanBackend/VulkanRasterizingDrawEntry.hpp"
#include "VulkanBackend/VulkanVertexStorage.hpp"

#include <Optick.h>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBuffers::VulkanCommandBuffers(const std::shared_ptr<VulkanDevice>& pDevice, uint32_t bufferCount, VkCommandBufferLevel level /*= VK_COMMAND_BUFFER_LEVEL_PRIMARY*/)
			: CommandBuffers(pDevice, bufferCount)
		{
			OPTICK_EVENT();

			// Create the command pool.
			VkCommandPoolCreateInfo commandPoolCreateInfo = {};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = nullptr;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = getDevice().as<VulkanDevice>()->getGraphicsQueue().getUnsafe().m_Family;

			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreateCommandPool(getDevice().as<VulkanDevice>()->getLogicalDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPool), "Failed to create the command pool!");

			// Allocate the command buffers.
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = m_CommandPool;
			allocateInfo.level = level;
			allocateInfo.commandBufferCount = bufferCount;

			m_CommandBuffers.resize(bufferCount);
			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkAllocateCommandBuffers(getDevice().as<VulkanDevice>()->getLogicalDevice(), &allocateInfo, m_CommandBuffers.data()), "Failed to allocate command buffers!");

			// Get the current command buffer.
			m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentIndex];

			// Create the fences.
			createFences();

			validate();
		}

		VulkanCommandBuffers::VulkanCommandBuffers(const std::shared_ptr<VulkanDevice>& pDevice, VkCommandBufferLevel level /*= VK_COMMAND_BUFFER_LEVEL_PRIMARY*/)
			: CommandBuffers(pDevice)
		{
			OPTICK_EVENT();

			// Create the command pool.
			VkCommandPoolCreateInfo commandPoolCreateInfo = {};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = nullptr;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = getDevice().as<VulkanDevice>()->getTransferQueue().getUnsafe().m_Family;

			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreateCommandPool(getDevice().as<VulkanDevice>()->getLogicalDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPool), "Failed to create the command pool!");

			// Allocate the command buffers.
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = m_CommandPool;
			allocateInfo.level = level;
			allocateInfo.commandBufferCount = 1;

			m_CommandBuffers.resize(1);
			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkAllocateCommandBuffers(getDevice().as<VulkanDevice>()->getLogicalDevice(), &allocateInfo, m_CommandBuffers.data()), "Failed to allocate command buffers!");

			// Get the current command buffer.
			m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentIndex];

			// Create the fences.
			createFences();

			validate();
		}

		VulkanCommandBuffers::~VulkanCommandBuffers()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanCommandBuffers::terminate()
		{
			OPTICK_EVENT();

			// Free the command buffers and destroy the pool.
			getDevice().as<VulkanDevice>()->getDeviceTable().vkFreeCommandBuffers(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
			getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyCommandPool(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_CommandPool, nullptr);

			// Destroy the fences.
			destroyFences();

			invalidate();
		}

		void VulkanCommandBuffers::begin(VkCommandBufferInheritanceInfo* pInheritanceInfo)
		{
			OPTICK_EVENT();

			// End recording if we are in the recording stage.
			if (isRecording())
				end();

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = nullptr;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = pInheritanceInfo;

			// Now we can create the command buffer.
			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkBeginCommandBuffer(m_CurrentCommandBuffer, &beginInfo), "Failed to begin command buffer recording!");
			m_IsRecording = true;
		}

		void VulkanCommandBuffers::bindWindow(const VulkanWindow& window, const VkClearValue& clearColors) const noexcept
		{
			OPTICK_EVENT();

			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = VK_NULL_HANDLE;
			renderPassBeginInfo.renderPass = window.getRenderPass();
			renderPassBeginInfo.framebuffer = window.getCurrentFramebuffer();
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ window.getWidth(), window.getHeight() };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearColors;

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdBeginRenderPass(m_CurrentCommandBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffers::unbindWindow() const
		{
			OPTICK_EVENT();

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdEndRenderPass(m_CurrentCommandBuffer);
		}

		void VulkanCommandBuffers::bindRenderTarget(const VulkanRasterizer& rasterizer, const std::vector<VkClearValue>& clearColors) const noexcept
		{
			OPTICK_EVENT();

			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = VK_NULL_HANDLE;
			renderPassBeginInfo.renderPass = rasterizer.getRenderPass();
			renderPassBeginInfo.framebuffer = rasterizer.getCurrentFramebuffer();
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ rasterizer.getWidth(), rasterizer.getHeight() };
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearColors.size());
			renderPassBeginInfo.pClearValues = clearColors.data();

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdBeginRenderPass(m_CurrentCommandBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffers::unbindRenderTarget() const
		{
			OPTICK_EVENT();

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdEndRenderPass(m_CurrentCommandBuffer);
		}

		void VulkanCommandBuffers::changeImageLayout(VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, uint32_t mipLevels /*= 1*/, uint32_t layers /*= 1*/) const
		{
			OPTICK_EVENT();

			// Create the memory barrier.
			VkImageMemoryBarrier memorybarrier = {};
			memorybarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			memorybarrier.srcAccessMask = 0;
			memorybarrier.dstAccessMask = 0;
			memorybarrier.oldLayout = currentLayout;
			memorybarrier.newLayout = newLayout;
			memorybarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memorybarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memorybarrier.image = image;
			memorybarrier.subresourceRange.aspectMask = aspectFlags;
			memorybarrier.subresourceRange.baseMipLevel = 0;
			memorybarrier.subresourceRange.levelCount = mipLevels;
			memorybarrier.subresourceRange.baseArrayLayer = 0;
			memorybarrier.subresourceRange.layerCount = layers;

			// Resolve the source access masks.
			switch (currentLayout)
			{
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_UNDEFINED:
				memorybarrier.srcAccessMask = 0;
				break;

			case VK_IMAGE_LAYOUT_PREINITIALIZED:
				memorybarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				//vMB.srcAccessMask = VK_ACCESS_;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;

			default:
				throw BackendError("Unsupported layout transition!");
			}

			// Resolve the destination access masks.
			switch (newLayout)
			{
			case VK_IMAGE_LAYOUT_UNDEFINED:
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				memorybarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				memorybarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				memorybarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				memorybarrier.dstAccessMask = memorybarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				memorybarrier.srcAccessMask |= VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				memorybarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;

			default:
				BackendError("Unsupported layout transition!");
			}

			// Resolve the pipeline stages.
			const auto sourceStage = Utility::GetPipelineStageFlags(memorybarrier.srcAccessMask);
			const auto destinationStage = Utility::GetPipelineStageFlags(memorybarrier.dstAccessMask);

			// Issue the commands. 
			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdPipelineBarrier(m_CurrentCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &memorybarrier);
		}

		void VulkanCommandBuffers::copyBuffer(VkBuffer srcBuffer, uint64_t size, uint64_t srcOffset, VkBuffer dstBuffer, uint64_t dstOffset) const noexcept
		{
			OPTICK_EVENT();

			VkBufferCopy bufferCopy = {};
			bufferCopy.size = size;
			bufferCopy.srcOffset = srcOffset;
			bufferCopy.dstOffset = dstOffset;

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdCopyBuffer(m_CurrentCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
		}

		void VulkanCommandBuffers::copyBufferToImage(VkBuffer srcBuffer, uint64_t bufferOffset, uint32_t bufferHeight, uint32_t bufferWidth, VkImage dstImage, VkImageLayout layout, VkExtent3D imageExtent, VkOffset3D imageOffset, VkImageSubresourceLayers imageSubresource) const noexcept
		{
			OPTICK_EVENT();

			VkBufferImageCopy imageCopy = {};
			imageCopy.imageExtent = imageExtent;
			imageCopy.imageOffset = imageOffset;
			imageCopy.imageSubresource = imageSubresource;
			imageCopy.bufferOffset = bufferOffset;
			imageCopy.bufferImageHeight = bufferHeight;
			imageCopy.bufferRowLength = bufferWidth;

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdCopyBufferToImage(m_CurrentCommandBuffer, srcBuffer, dstImage, layout, 1, &imageCopy);
		}

		void VulkanCommandBuffers::copyImageToBuffer(VkImage srcImage, VkImageLayout layout, VkExtent3D imageExtent, VkOffset3D imageOffset, VkImageSubresourceLayers imageSubresource, VkBuffer dstBuffer, uint64_t bufferOffset, uint32_t bufferHeight, uint32_t bufferWidth) const noexcept
		{
			OPTICK_EVENT();

			VkBufferImageCopy imageCopy = {};
			imageCopy.imageExtent = imageExtent;
			imageCopy.imageOffset = imageOffset;
			imageCopy.imageSubresource = imageSubresource;
			imageCopy.bufferOffset = bufferOffset;
			imageCopy.bufferImageHeight = bufferHeight;
			imageCopy.bufferRowLength = bufferWidth;

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdCopyImageToBuffer(m_CurrentCommandBuffer, srcImage, layout, dstBuffer, 1, &imageCopy);
		}

		void VulkanCommandBuffers::bindRasterizingPipeline(VkPipeline pipeline) const noexcept
		{
			OPTICK_EVENT();

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		}

		void VulkanCommandBuffers::bindVertexBuffers(const VulkanVertexStorage& vertexStorage, const std::vector<VertexInput>& inputs) const noexcept
		{
			OPTICK_EVENT();

			std::vector<VkBuffer> buffers;
			buffers.reserve(inputs.size());

			for (const auto& input : inputs)
			{
				const auto& pBuffer = vertexStorage.getBuffer(input.m_Attribute);

				if (pBuffer)
					buffers.emplace_back(pBuffer->getBuffer());
			}

			const auto offsets = std::vector<VkDeviceSize>(buffers.size(), 0);
			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, static_cast<uint32_t>(buffers.size()), buffers.data(), offsets.data());
		}

		void VulkanCommandBuffers::bindIndexBuffer(const VkBuffer buffer) const noexcept
		{
			OPTICK_EVENT();

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdBindIndexBuffer(m_CurrentCommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
		}

		void VulkanCommandBuffers::drawIndexed(uint64_t indexCount, uint64_t indexOffset, uint64_t instanceCount, uint64_t vertexOffset) const noexcept
		{
			OPTICK_EVENT();

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdDrawIndexed(m_CurrentCommandBuffer, static_cast<uint32_t>(indexCount), static_cast<uint32_t>(instanceCount), static_cast<uint32_t>(indexOffset), static_cast<int32_t>(vertexOffset), 0);
		}

		void VulkanCommandBuffers::bindDescriptor(const VulkanRasterizingPipeline* pPipeline, VkDescriptorSet descriptorSet) const noexcept
		{
			OPTICK_EVENT();

			getDevice().as<VulkanDevice>()->getDeviceTable().vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->getProgram()->as<VulkanRasterizingProgram>()->getPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
		}

		void VulkanCommandBuffers::end()
		{
			OPTICK_EVENT();

			// Just return if we are not recording.
			if (!m_IsRecording)
				return;

			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkEndCommandBuffer(m_CurrentCommandBuffer), "Failed to end command buffer recording!");
			m_IsRecording = false;
		}

		void VulkanCommandBuffers::submit(VkSemaphore renderFinishedSemaphore, VkSemaphore inFlightSemaphore, VkPipelineStageFlags waitStageMask /*= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT*/)
		{
			OPTICK_EVENT();

			// Create the submit info structure.
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &inFlightSemaphore;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_CurrentCommandBuffer;
			submitInfo.pWaitDstStageMask = &waitStageMask;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

			// Submit the queue.
			auto& fence = m_CommandFences[m_CurrentIndex];
			fence.m_IsFree = false;

			auto pVulkanDevice = getDevice().as<VulkanDevice>();
			pVulkanDevice->getGraphicsQueue().apply([this, pVulkanDevice, submitInfo, fence](VulkanQueue& queue)
				{
					FLINT_VK_ASSERT(pVulkanDevice->getDeviceTable().vkQueueSubmit(queue.m_Queue, 1, &submitInfo, fence.m_Fence), "Failed to submit the queue!");
				}
			);
		}

		void VulkanCommandBuffers::submitGraphics(VkPipelineStageFlags waitStageMask /*= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT*/)
		{
			OPTICK_EVENT();

			// Create the submit info structure.
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_CurrentCommandBuffer;
			submitInfo.pWaitDstStageMask = &waitStageMask;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;

			// Submit the queue.
			auto& fence = m_CommandFences[m_CurrentIndex];
			fence.m_IsFree = false;

			auto pVulkanDevice = getDevice().as<VulkanDevice>();
			pVulkanDevice->getGraphicsQueue().apply([this, pVulkanDevice, submitInfo, fence](VulkanQueue& queue)
				{
					FLINT_VK_ASSERT(pVulkanDevice->getDeviceTable().vkQueueSubmit(queue.m_Queue, 1, &submitInfo, fence.m_Fence), "Failed to submit the queue!");
				}
			);
		}

		void VulkanCommandBuffers::submitTransfer()
		{
			OPTICK_EVENT();

			const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

			// Create the submit info structure.
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_CurrentCommandBuffer;
			submitInfo.pWaitDstStageMask = &waitStageMask;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;

			// Submit the queue.
			auto& fence = m_CommandFences[m_CurrentIndex];
			fence.m_IsFree = false;

			auto pVulkanDevice = getDevice().as<VulkanDevice>();
			pVulkanDevice->getTransferQueue().apply([this, pVulkanDevice, submitInfo, fence](VulkanQueue& queue)
				{
					FLINT_VK_ASSERT(pVulkanDevice->getDeviceTable().vkQueueSubmit(queue.m_Queue, 1, &submitInfo, fence.m_Fence), "Failed to submit the queue!");
				}
			);
		}

		void VulkanCommandBuffers::submitCompute()
		{
			OPTICK_EVENT();

			const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

			// Create the submit info structure.
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_CurrentCommandBuffer;
			submitInfo.pWaitDstStageMask = &waitStageMask;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;

			// Submit the queue.
			auto& fence = m_CommandFences[m_CurrentIndex];
			fence.m_IsFree = false;

			auto pVulkanDevice = getDevice().as<VulkanDevice>();
			pVulkanDevice->getComputeQueue().apply([this, pVulkanDevice, submitInfo, fence](VulkanQueue& queue)
				{
					FLINT_VK_ASSERT(pVulkanDevice->getDeviceTable().vkQueueSubmit(queue.m_Queue, 1, &submitInfo, fence.m_Fence), "Failed to submit the queue!");
				}
			);
		}

		void VulkanCommandBuffers::finishExecution()
		{
			OPTICK_EVENT();

			// End recording if we are in the recording stage.
			if (isRecording())
				return;

			auto& fence = m_CommandFences[m_CurrentIndex];

			// If the current fence is not free, we can wait.
			if (!fence.m_IsFree)
			{
				FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkWaitForFences(getDevice().as<VulkanDevice>()->getLogicalDevice(), 1, &m_CommandFences[m_CurrentIndex].m_Fence, VK_TRUE, std::numeric_limits<uint64_t>::max()), "Failed to wait for the fence!");
				FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkResetFences(getDevice().as<VulkanDevice>()->getLogicalDevice(), 1, &m_CommandFences[m_CurrentIndex].m_Fence), "Failed to reset fence!");
			}
		}

		void VulkanCommandBuffers::next()
		{
			OPTICK_EVENT();

			m_CurrentIndex = ++m_CurrentIndex % m_CommandBuffers.size();
			m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentIndex];
		}

		void VulkanCommandBuffers::resetIndex()
		{
			OPTICK_EVENT();

			end();
			m_CurrentIndex = 0;
			m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentIndex];
		}

		void VulkanCommandBuffers::createFences()
		{
			OPTICK_EVENT();

			VkFenceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;

			m_CommandFences.reserve(m_CommandBuffers.size());
			for (uint32_t i = 0; i < m_CommandBuffers.size(); i++)
				FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreateFence(getDevice().as<VulkanDevice>()->getLogicalDevice(), &createInfo, nullptr, &m_CommandFences.emplace_back().m_Fence), "Failed to create fence!");
		}

		void VulkanCommandBuffers::destroyFences()
		{
			OPTICK_EVENT();

			for (const auto fence : m_CommandFences)
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyFence(getDevice().as<VulkanDevice>()->getLogicalDevice(), fence.m_Fence, nullptr);
		}
	}
}