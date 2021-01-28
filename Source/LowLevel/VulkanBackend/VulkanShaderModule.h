// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.h"
#include "Core/Backend/ShaderDigest.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class ShaderModule final : VulkanDeviceBoundObject {
		public:
			ShaderModule() {}
			~ShaderModule() {}

			void Initialize(VulkanDevice* pDevice, const ShaderDigest& digest);
			void Terminate() override final;

		private:
			void SetupResources(const ShaderDigest& digest);

		public:
			VkShaderModule vShaderModule = VK_NULL_HANDLE;
			VkShaderStageFlags vShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

			std::vector<VkDescriptorSetLayoutBinding> mBindings;
			std::vector<VkDescriptorPoolSize> mPoolSizes;
			std::vector<VkPushConstantRange> mConstantRange;
		};
	}
}