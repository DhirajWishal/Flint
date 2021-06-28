// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		struct ShaderDescriptor
		{
			std::vector<Backend::ShaderResource> mResources;
			std::vector<Backend::ShaderAttribute> mInputAttributes;
			std::vector<Backend::ShaderAttribute> mOutputAttributes;
		};

		class VulkanShader
		{
		public:
			VulkanShader(VulkanDevice& device, VkShaderStageFlags stageFlags, const std::filesystem::path& path, Backend::ShaderCodeType type);
			VulkanShader(VulkanDevice& device, VkShaderStageFlags stageFlags, const std::vector<UI32>& code, Backend::ShaderCodeType type);
			VulkanShader(VulkanDevice& device, VkShaderStageFlags stageFlags, const std::string& code, Backend::ShaderCodeType type);

			void Terminate();
			VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;
			ShaderDescriptor PerformReflection() const;

			VkShaderModule GetModule() const { return vModule; }

		private:
			void CreateShaderModule();

		private:
			VulkanDevice& vDevice;
			std::vector<UI32> mShaderCode;

			VkShaderModule vModule = VK_NULL_HANDLE;
			VkShaderStageFlags vStageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		};
	}
}