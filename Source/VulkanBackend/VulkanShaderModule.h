// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.h"
#include "Core/Backend/FShaderDigest.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanShaderModule final : public VulkanDeviceBoundObject, public BackendObject {
		public:
			using DeviceType = VulkanDevice;

		public:
			VulkanShaderModule(VulkanDevice* pDevice, const ShaderDigest& digest);
			~VulkanShaderModule();

			VkPipelineShaderStageCreateInfo GetStage() const;

		private:
			void SetupResources(const ShaderDigest& digest);

		public:
			VulkanDevice* pDevice = nullptr;

			VkShaderModule vShaderModule = VK_NULL_HANDLE;
			VkShaderStageFlags vShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

			std::vector<VkDescriptorSetLayoutBinding> mBindings;
			std::vector<VkDescriptorPoolSize> mPoolSizes;
			std::vector<VkPushConstantRange> mConstantRange;
		};
	}
}