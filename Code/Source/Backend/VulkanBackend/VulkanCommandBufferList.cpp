// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBufferList.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBufferList::VulkanCommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount) : CommandBufferList(pDevice, bufferCount)
		{
			FLINT_SETUP_PROFILER();

			auto vDevice = pDevice->StaticCast<VulkanDevice>();

			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = vDevice.GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vPoolCI, nullptr, &vCommandPool));

			VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandPool = vCommandPool;
			vAI.commandBufferCount = bufferCount;

			vCommandBuffers.resize(bufferCount);
			FLINT_VK_ASSERT(vkAllocateCommandBuffers(vDevice.GetLogicalDevice(), &vAI, vCommandBuffers.data()));
		}

		VulkanCommandBufferList::VulkanCommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pParent)
			: CommandBufferList(pDevice, bufferCount, pParent)
		{
			FLINT_SETUP_PROFILER();

			auto vDevice = pDevice->StaticCast<VulkanDevice>();

			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = vDevice.GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vPoolCI, nullptr, &vCommandPool));

			VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			vAI.commandPool = vCommandPool;
			vAI.commandBufferCount = bufferCount;

			vCommandBuffers.resize(bufferCount);
			FLINT_VK_ASSERT(vkAllocateCommandBuffers(vDevice.GetLogicalDevice(), &vAI, vCommandBuffers.data()));
		}

		void VulkanCommandBufferList::BeginBufferRecording(UI32 index)
		{
			FLINT_SETUP_PROFILER();

			mCurrentBufferIndex = index;
			vCurrentBuffer = vCommandBuffers[index];

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

			FLINT_VK_ASSERT(vkBeginCommandBuffer(vCurrentBuffer, &vBeginInfo));
		}

		void VulkanCommandBufferList::BeginBufferRecording(UI32 index, const std::shared_ptr<CommandBufferList> pParent)
		{
			FLINT_SETUP_PROFILER();

			mCurrentBufferIndex = index;
			vCurrentBuffer = vCommandBuffers[index];

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

			FLINT_VK_ASSERT(vkBeginCommandBuffer(vCurrentBuffer, &vBeginInfo));
		}

		void VulkanCommandBufferList::BeginNextBufferRecording()
		{
			FLINT_SETUP_PROFILER();

			mCurrentBufferIndex++;
			if (mCurrentBufferIndex >= mBufferCount) mCurrentBufferIndex = 0;

			BeginBufferRecording(mCurrentBufferIndex);
		}

		void VulkanCommandBufferList::BeginNextBufferRecording(const std::shared_ptr<CommandBufferList> pParent)
		{
			FLINT_SETUP_PROFILER();

			mCurrentBufferIndex++;
			if (mCurrentBufferIndex >= mBufferCount) mCurrentBufferIndex = 0;

			BeginBufferRecording(mCurrentBufferIndex, pParent);
		}

		void VulkanCommandBufferList::BindRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			VulkanScreenBoundRenderTarget& vRenderTarget = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();
			FColor4D clearColors = vRenderTarget.GetClearColor();

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFrameBuffer(GetCurrentBufferIndex());
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			vkCmdBeginRenderPass(GetCurrentCommandBuffer(), &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBufferList::UnbindRenderTarget()
		{
			vkCmdEndRenderPass(GetCurrentCommandBuffer());
		}

		void VulkanCommandBufferList::BindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline>& pGraphicsPipeline)
		{
			vkCmdBindPipeline(GetCurrentCommandBuffer(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipeline());
		}

		void VulkanCommandBufferList::EndBufferRecording()
		{
			FLINT_VK_ASSERT(vkEndCommandBuffer(vCurrentBuffer));
		}

		void VulkanCommandBufferList::Terminate()
		{
			auto vDevice = pDevice->StaticCast<VulkanDevice>();
			vkFreeCommandBuffers(vDevice.GetLogicalDevice(), vCommandPool, static_cast<UI32>(vCommandBuffers.size()), vCommandBuffers.data());
			vkDestroyCommandPool(vDevice.GetLogicalDevice(), vCommandPool, nullptr);
		}

		void VulkanCommandBufferList::ClearBuffers()
		{
			for (VkCommandBuffer vBuffer : vCommandBuffers)
				vkResetCommandBuffer(vBuffer, 0);
		}

		VkCommandBufferInheritanceInfo VulkanCommandBufferList::GetInheritanceInfo() const
		{
			return VkCommandBufferInheritanceInfo();
		}
	}
}