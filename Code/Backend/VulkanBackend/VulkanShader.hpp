// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanShader final : public Backend::Shader
		{
		public:
			VulkanShader(const std::shared_ptr<Backend::Device>& pDevice, Backend::ShaderType type, const std::filesystem::path& path, Backend::ShaderCodeType codeType);
			VulkanShader(const std::shared_ptr<Backend::Device>& pDevice, Backend::ShaderType type, const std::vector<UI32>& code, Backend::ShaderCodeType codeType);
			VulkanShader(const std::shared_ptr<Backend::Device>& pDevice, Backend::ShaderType type, const std::string& code, Backend::ShaderCodeType codeType);

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