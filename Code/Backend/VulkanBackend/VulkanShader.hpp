// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanShader final : public Shader
		{
		public:
			VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::filesystem::path& path, ShaderCodeType codeType);
			VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::vector<UI32>& code, ShaderCodeType codeType);
			VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::string& code, ShaderCodeType codeType);

			virtual void Terminate() override final;

			VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;
			void PerformReflection();

			VkShaderModule GetModule() const { return vModule; }

		private:
			void ResolveShaderStage();
			void CreateShaderModule();

		private:
			std::vector<UI32> mShaderCode;

			VkShaderModule vModule = VK_NULL_HANDLE;
			VkShaderStageFlags vStageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		};
	}
}