// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.h"

#include "Buffers/VulkanUniformBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanResourceContainer final : public VulkanDeviceBoundObject {
		public:
			VulkanResourceContainer() {}
			~VulkanResourceContainer() {}

			void Initialize(VulkanDevice* pDevice, const std::vector<VkDescriptorPoolSize>& sizes, VkDescriptorSetLayout vLayout);
			virtual void Terminate() override final;

		public:
			void RegisterBuffer(VulkanUniformBuffer* pUniformBuffer);

		public:
			std::unordered_map<String, VulkanUniformBuffer> mUniformBuffers;

		private:
			VkDescriptorPool vPool = VK_NULL_HANDLE;
			VkDescriptorSet vSet = VK_NULL_HANDLE;
		};
	}
}