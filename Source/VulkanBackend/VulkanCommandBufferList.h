// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\CommandBufferList.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferList final : public Backend::CommandBufferList<VulkanDevice, VulkanBuffer> {
		public:
			VulkanCommandBufferList() {}
			~VulkanCommandBufferList() {}

			virtual void Initialize(DeviceType* pDevice, UI64 bufferCount) override final;
			virtual void Terminate() override final;

			virtual void BeginBuffer(UI64 index) override final;
			virtual void EndBuffer() override final;

			virtual void BindVertexBuffer(const BufferType& buffer, UI64 firstBinding, UI64 bindingCount) override final;

			VkCommandPool GetCommandPool() const { return vCommandPool; }
			VkCommandBuffer GetCurrentBuffer() const { return vBuffers[GetIndex()]; }

		private:
			VkCommandPool vCommandPool = VK_NULL_HANDLE;
			std::vector<VkCommandBuffer> vBuffers = {};
		};
	}
}