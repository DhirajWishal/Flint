// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanShader final : public Shader, public std::enable_shared_from_this<VulkanShader>
		{
		public:
			VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::filesystem::path& path);
			VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::vector<UI32>& code);
			VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::string& code);
			~VulkanShader() { if (!bIsTerminated) Terminate(); }

			virtual void Terminate() override final;

			VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;
			void PerformReflection();

			VkShaderModule GetModule() const { return vModule; }
			const std::vector<VkDescriptorSetLayoutBinding> GetResourceBindings() const { return mBindings; }
			const std::vector<VkDescriptorPoolSize> GetPoolSizes() const { return mSizes; }
			const std::vector<VkPushConstantRange> GetPushConstantRanges() const { return mConstantRanges; }

		private:
			void ResolveShaderStage();
			void CreateShaderModule();

		private:
			std::vector<UI32> mShaderCode;
			std::vector<VkDescriptorSetLayoutBinding> mBindings;
			std::vector<VkDescriptorPoolSize> mSizes;
			std::vector<VkPushConstantRange> mConstantRanges;

			VkShaderModule vModule = VK_NULL_HANDLE;

			VkShaderStageFlags vStageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		};
	}
}