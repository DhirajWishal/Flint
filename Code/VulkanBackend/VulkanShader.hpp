// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		struct DescriptorSetInfo
		{
			std::vector<VkDescriptorSetLayoutBinding> mLayoutBindings;
			std::vector<VkDescriptorPoolSize> mPoolSizes;
		};

		class VulkanShader final : public Shader, public std::enable_shared_from_this<VulkanShader>
		{
		public:
			VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::filesystem::path& path);
			VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::vector<UI32>& code);
			VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::string& code);
			~VulkanShader() { if (!bIsTerminated) Terminate(); }

			virtual void Reload(const std::filesystem::path& path) override final;
			virtual void Reload(const std::vector<UI32>& code) override final;
			virtual void Reload(const std::string& code) override final;

			virtual void Terminate() override final;

			VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;
			void PerformReflection();

			VkShaderModule GetModule() const { return vModule; }
			const std::vector<VkPushConstantRange> GetPushConstantRanges() const { return mConstantRanges; }
			const std::unordered_map<UI32, DescriptorSetInfo> GetDescriptorSetMap() const { return mDescriptorSetMap; }

		private:
			void ResolveShaderStage();
			void CreateShaderModule();

		private:
			std::vector<UI32> mShaderCode;
			std::vector<VkPushConstantRange> mConstantRanges;

			std::unordered_map<UI32, DescriptorSetInfo> mDescriptorSetMap;

			VkShaderModule vModule = VK_NULL_HANDLE;

			VkShaderStageFlags vStageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		};
	}
}