// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/CommandBufferList.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferList final : public CommandBufferList
		{
		public:
			VulkanCommandBufferList(Device &device, const UI32 bufferCount): CommandBufferList(device, bufferCount)
			{
				auto vDevice = device.StaticCast<VulkanDevice>();

				VkCommandPoolCreateInfo vPoolCI = {};
				vPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				vPoolCI.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				vPoolCI.pNext = VK_NULL_HANDLE;
				vPoolCI.queueFamilyIndex = vDevice.GetQueue().mTransferFamily.value();

				FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vPoolCI, nullptr, &vCommandPool));

				VkCommandBufferAllocateInfo vAI = {};
				vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				vAI.pNext = VK_NULL_HANDLE;
				vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				vAI.commandPool = vCommandPool;
				vAI.commandBufferCount = bufferCount;

				vCommandBuffers.resize(bufferCount);
				FLINT_VK_ASSERT(vkAllocateCommandBuffers(vDevice.GetLogicalDevice(), &vAI, vCommandBuffers.data()));
			}

			VulkanCommandBufferList(Device &device, const UI32 bufferCount, CommandBufferList &parent) : CommandBufferList(device, bufferCount, parent)
			{
				auto vDevice = device.StaticCast<VulkanDevice>();

				VkCommandPoolCreateInfo vPoolCI = {};
				vPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				vPoolCI.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				vPoolCI.pNext = VK_NULL_HANDLE;
				vPoolCI.queueFamilyIndex = vDevice.GetQueue().mTransferFamily.value();

				FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vPoolCI, nullptr, &vCommandPool));

				VkCommandBufferAllocateInfo vAI = {};
				vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				vAI.pNext = VK_NULL_HANDLE;
				vAI.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
				vAI.commandPool = vCommandPool;
				vAI.commandBufferCount = bufferCount;

				vCommandBuffers.resize(bufferCount);
				FLINT_VK_ASSERT(vkAllocateCommandBuffers(vDevice.GetLogicalDevice(), &vAI, vCommandBuffers.data()));
			}

			virtual void BeginBufferRecording(UI32 index) override final

			{
				mCurrentBufferIndex = index;
				vCurrentBuffer = vCommandBuffers[index];

				VkCommandBufferBeginInfo vBeginInfo = {};
				vBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				vBeginInfo.pNext = VK_NULL_HANDLE;
				vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
				vBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

				FLINT_VK_ASSERT(vkBeginCommandBuffer(vCurrentBuffer, &vBeginInfo));
			}

			virtual void BeginBufferRecording(UI32 index, CommandBufferList &parent) override final
			{
				mCurrentBufferIndex = index;
				vCurrentBuffer = vCommandBuffers[index];

				VkCommandBufferBeginInfo vBeginInfo = {};
				vBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				vBeginInfo.pNext = VK_NULL_HANDLE;
				vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
				vBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

				FLINT_VK_ASSERT(vkBeginCommandBuffer(vCurrentBuffer, &vBeginInfo));
			}

			virtual void BeginNextBufferRecording() override final
			{
				mCurrentBufferIndex++;
				if (mCurrentBufferIndex >= mBufferCount)
					mCurrentBufferIndex = 0;

				BeginBufferRecording(mCurrentBufferIndex);
			}

			virtual void BeginNextBufferRecording(CommandBufferList &parent) override final
			{
				mCurrentBufferIndex++;
				if (mCurrentBufferIndex >= mBufferCount)
					mCurrentBufferIndex = 0;

				BeginBufferRecording(mCurrentBufferIndex, parent);
			}

			virtual void EndBufferRecording() override final
			{
				FLINT_VK_ASSERT(vkEndCommandBuffer(vCurrentBuffer));
			}

			virtual void Terminate() override final
			{
				auto vDevice = mDevice.StaticCast<VulkanDevice>();
				vkFreeCommandBuffers(vDevice.GetLogicalDevice(), vCommandPool, static_cast<UI32>(vCommandBuffers.size()), vCommandBuffers.data());
				vkDestroyCommandPool(vDevice.GetLogicalDevice(), vCommandPool, nullptr);
			}

			VkCommandPool GetCommandPool() const { return vCommandPool; }
			VkCommandBuffer GetCurrentCommandBuffer() const { return vCurrentBuffer; }
			VkCommandBuffer GetCommandBuffer(UI32 index) const { return vCommandBuffers[index]; }
			std::vector<VkCommandBuffer> GetCommandBuffers() const { return vCommandBuffers; }

			VkCommandBufferInheritanceInfo GetInheritanceInfo() const
			{
				return VkCommandBufferInheritanceInfo();
			}

		private:
			VkCommandPool vCommandPool = VK_NULL_HANDLE;
			std::vector<VkCommandBuffer> vCommandBuffers = {};

			VkCommandBuffer vCurrentBuffer = VK_NULL_HANDLE;
		};
	}
}