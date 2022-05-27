// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBuffers.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanWindow.hpp"
#include "VulkanBackend/VulkanRasterizer.hpp"
#include "VulkanBackend/VulkanGeometryStore.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBuffers::VulkanCommandBuffers(VulkanEngine& engine, uint32_t bufferCount, VkCommandBufferLevel level /*= VK_COMMAND_BUFFER_LEVEL_PRIMARY*/)
			: m_Engine(engine)
		{
			// Create the command pool.
			VkCommandPoolCreateInfo commandPoolCreateInfo = {};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = nullptr;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = m_Engine.getGraphicsQueue().m_Family;

			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkCreateCommandPool(m_Engine.getLogicalDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPool), "Failed to create the command pool!");

			// Allocate the command buffers.
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = m_CommandPool;
			allocateInfo.level = level;
			allocateInfo.commandBufferCount = bufferCount;

			m_CommandBuffers.resize(bufferCount);
			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkAllocateCommandBuffers(m_Engine.getLogicalDevice(), &allocateInfo, m_CommandBuffers.data()), "Failed to allocate command buffers!");

			// Get the current command buffer.
			m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentIndex];

			// Create the fences.
			createFences();
		}

		VulkanCommandBuffers::VulkanCommandBuffers(VulkanEngine& engine, VkCommandBufferLevel level /*= VK_COMMAND_BUFFER_LEVEL_PRIMARY*/)
			: m_Engine(engine)
		{
			// Create the command pool.
			VkCommandPoolCreateInfo commandPoolCreateInfo = {};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = nullptr;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = m_Engine.getTransferQueue().m_Family;

			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkCreateCommandPool(m_Engine.getLogicalDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPool), "Failed to create the command pool!");

			// Allocate the command buffers.
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = m_CommandPool;
			allocateInfo.level = level;
			allocateInfo.commandBufferCount = 1;

			m_CommandBuffers.resize(1);
			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkAllocateCommandBuffers(m_Engine.getLogicalDevice(), &allocateInfo, m_CommandBuffers.data()), "Failed to allocate command buffers!");

			// Get the current command buffer.
			m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentIndex];

			// Create the fences.
			createFences();
		}

		VulkanCommandBuffers::~VulkanCommandBuffers()
		{
			// Free the command buffers and destroy the pool.
			m_Engine.getDeviceTable().vkFreeCommandBuffers(m_Engine.getLogicalDevice(), m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
			m_Engine.getDeviceTable().vkDestroyCommandPool(m_Engine.getLogicalDevice(), m_CommandPool, nullptr);

			// Destroy the fences.
			destroyFences();
		}

		void VulkanCommandBuffers::begin(VkCommandBufferInheritanceInfo* pInheritanceInfo)
		{
			// End recording if we are in the recording stage.
			if (isRecording())
				end();

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = nullptr;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = pInheritanceInfo;

			// Finish previous execution before we run this.
			finishExecution();

			// Now we can create the command buffer.
			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkBeginCommandBuffer(m_CurrentCommandBuffer, &beginInfo), "Failed to begin command buffer recording!");
			m_IsRecording = true;
		}

		void VulkanCommandBuffers::bindWindow(const VulkanWindow& window, const std::vector<VkClearValue>& clearColors) const noexcept
		{
			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = VK_NULL_HANDLE;
			renderPassBeginInfo.renderPass = window.getRenderPass();
			renderPassBeginInfo.framebuffer = window.getCurrentFramebuffer();
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ window.getWidth(), window.getHeight() };
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearColors.size());
			renderPassBeginInfo.pClearValues = clearColors.data();

			m_Engine.getDeviceTable().vkCmdBeginRenderPass(m_CurrentCommandBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffers::unbindWindow() const
		{
			m_Engine.getDeviceTable().vkCmdEndRenderPass(m_CurrentCommandBuffer);
		}

		void VulkanCommandBuffers::bindRenderTarget(const VulkanRasterizer& rasterizer, const std::vector<VkClearValue>& clearColors) const noexcept
		{
			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = VK_NULL_HANDLE;
			renderPassBeginInfo.renderPass = rasterizer.getRenderPass();
			renderPassBeginInfo.framebuffer = rasterizer.getCurrentFramebuffer();
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ rasterizer.getWidth(), rasterizer.getHeight() };
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearColors.size());
			renderPassBeginInfo.pClearValues = clearColors.data();

			m_Engine.getDeviceTable().vkCmdBeginRenderPass(m_CurrentCommandBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffers::unbindRenderTarget() const
		{
			m_Engine.getDeviceTable().vkCmdEndRenderPass(m_CurrentCommandBuffer);
		}

		void VulkanCommandBuffers::changeImageLayout(VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags) const
		{
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
			memorybarrier.subresourceRange.levelCount = 1;
			memorybarrier.subresourceRange.baseArrayLayer = 0;
			memorybarrier.subresourceRange.layerCount = 1;

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
			m_Engine.getDeviceTable().vkCmdPipelineBarrier(m_CurrentCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &memorybarrier);
		}

		void VulkanCommandBuffers::copyBuffer(VkBuffer srcBuffer, uint64_t size, uint64_t srcOffset, VkBuffer dstBuffer, uint64_t dstOffset) const noexcept
		{
			VkBufferCopy bufferCopy = {};
			bufferCopy.size = size;
			bufferCopy.srcOffset = srcOffset;
			bufferCopy.dstOffset = dstOffset;

			m_Engine.getDeviceTable().vkCmdCopyBuffer(m_CurrentCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
		}

		void VulkanCommandBuffers::bindIndexBuffer(const VulkanGeometryStore& geometryStore) const noexcept
		{
			m_Engine.getDeviceTable().vkCmdBindIndexBuffer(m_CurrentCommandBuffer, geometryStore.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}

		void VulkanCommandBuffers::bindGraphicsPipeline(const VulkanGraphicsPipeline& pipeline) const noexcept
		{
			m_Engine.getDeviceTable().vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipeline());
		}

		void VulkanCommandBuffers::bindDescriptor(const VulkanGraphicsPipeline& pipeline, VkDescriptorSet descriptorSet) const noexcept
		{
			m_Engine.getDeviceTable().vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
		}

		void VulkanCommandBuffers::drawMesh(const VulkanGeometryStore& geometryStore, const Mesh& mesh) const noexcept
		{
			const VkDeviceSize offset = mesh.getVertexOffset();
			m_Engine.getDeviceTable().vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, 1, geometryStore.getVertexBufferPtr(), &offset);
			m_Engine.getDeviceTable().vkCmdDrawIndexed(m_CurrentCommandBuffer, static_cast<uint32_t>(mesh.getIndexCount()), 1, static_cast<uint32_t>(mesh.getOffsetIndexCount()), 0, 0);
		}

		void VulkanCommandBuffers::end()
		{
			// Just return if we are not recording.
			if (!m_IsRecording)
				return;

			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkEndCommandBuffer(m_CurrentCommandBuffer), "Failed to end command buffer recording!");
			m_IsRecording = false;
		}

		void VulkanCommandBuffers::submit(VkSemaphore renderFinishedSemaphore, VkSemaphore inFlightSemaphore, VkPipelineStageFlags waitStageMask /*= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT*/)
		{
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
			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkQueueSubmit(m_Engine.getGraphicsQueue().m_Queue, 1, &submitInfo, fence.m_Fence), "Failed to submit the queue!");
			fence.m_IsFree = false;
		}

		void VulkanCommandBuffers::submitGraphics(VkPipelineStageFlags waitStageMask /*= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT*/)
		{
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

			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkQueueSubmit(m_Engine.getGraphicsQueue().m_Queue, 1, &submitInfo, fence.m_Fence), "Failed to submit the queue!");
		}

		void VulkanCommandBuffers::submitTransfer()
		{
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

			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkQueueSubmit(m_Engine.getTransferQueue().m_Queue, 1, &submitInfo, fence.m_Fence), "Failed to submit the queue!");
		}

		void VulkanCommandBuffers::submitCompute()
		{
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

			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkQueueSubmit(m_Engine.getComputeQueue().m_Queue, 1, &submitInfo, fence.m_Fence), "Failed to submit the queue!");
		}

		void VulkanCommandBuffers::finishExecution()
		{
			auto& fence = m_CommandFences[m_CurrentIndex];

			// If the current fence is not free, we can wait.
			if (!fence.m_IsFree)
			{
				FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkWaitForFences(m_Engine.getLogicalDevice(), 1, &m_CommandFences[m_CurrentIndex].m_Fence, VK_TRUE, std::numeric_limits<uint64_t>::max()), "Failed to wait for the fence!");
				FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkResetFences(m_Engine.getLogicalDevice(), 1, &m_CommandFences[m_CurrentIndex].m_Fence), "Failed to reset fence!");
			}
		}

		void VulkanCommandBuffers::next()
		{
			m_CurrentIndex = ++m_CurrentIndex % m_CommandBuffers.size();
			m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentIndex];
		}

		void VulkanCommandBuffers::resetIndex()
		{
			end();
			m_CurrentIndex = 0;
			m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentIndex];
		}

		void VulkanCommandBuffers::createFences()
		{
			VkFenceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;

			m_CommandFences.reserve(m_CommandBuffers.size());
			for (uint32_t i = 0; i < m_CommandBuffers.size(); i++)
				FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkCreateFence(m_Engine.getLogicalDevice(), &createInfo, nullptr, &m_CommandFences.emplace_back().m_Fence), "Failed to create fence!");
		}

		void VulkanCommandBuffers::destroyFences()
		{
			for (const auto fence : m_CommandFences)
				m_Engine.getDeviceTable().vkDestroyFence(m_Engine.getLogicalDevice(), fence.m_Fence, nullptr);
		}
	}
}