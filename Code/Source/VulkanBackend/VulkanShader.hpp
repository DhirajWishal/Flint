// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Shader.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		struct ShaderDescriptor
		{
			std::vector<ShaderResource> mResources;
			std::vector<ShaderAttribute> mInputAttributes;
			std::vector<ShaderAttribute> mOutputAttributes;
		};

		class VulkanShader
		{
		public:
			VulkanShader(VulkanDevice& device, VkShaderStageFlags stageFlags, const std::filesystem::path& path, ShaderCodeType type);
			VulkanShader(VulkanDevice& device, VkShaderStageFlags stageFlags, const std::vector<UI32>& code, ShaderCodeType type);
			VulkanShader(VulkanDevice& device, VkShaderStageFlags stageFlags, const std::string& code, ShaderCodeType type);

			VkShaderModule GetModule() const { return vModule; }

			ShaderDescriptor PerformReflection() const;

		private:
			void CreateShaderModule();

		private:
			VulkanDevice& vDevice;
			std::vector<UI32> mShaderCode;

			VkShaderModule vModule = VK_NULL_HANDLE;
		};
	}
}