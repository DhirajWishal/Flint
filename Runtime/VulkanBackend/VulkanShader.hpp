// Copyright 2021-2022 Dhiraj Wishal
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
			VulkanShader(Device* pDevice, const ShaderType type, const std::filesystem::path& path);
			VulkanShader(Device* pDevice, const ShaderType type, const std::vector<uint32_t>& code);
			VulkanShader(Device* pDevice, const ShaderType type, const std::string& code);
			~VulkanShader() { if (!bIsTerminated) Terminate(); }

			virtual void CreateCache(const std::filesystem::path& path) const override;
			virtual void Reload(const std::filesystem::path& path) override;
			virtual void Reload(const std::vector<uint32_t>& code) override;
			virtual void Reload(const std::string& code) override;

			virtual void Terminate() override;

			VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;
			void PerformReflection();

			VkShaderModule GetModule() const { return vModule; }
			const std::vector<VkPushConstantRange> GetPushConstantRanges() const { return mConstantRanges; }
			const std::unordered_map<uint32_t, DescriptorSetInfo> GetDescriptorSetMap() const { return mDescriptorSetMap; }

		private:
			void ResolveShaderStage();
			void CreateShaderModule();

		private:
			std::vector<uint32_t> mShaderCode;
			std::vector<VkPushConstantRange> mConstantRanges;

			std::unordered_map<uint32_t, DescriptorSetInfo> mDescriptorSetMap;

			VkShaderModule vModule = VK_NULL_HANDLE;

			VkShaderStageFlags vStageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		};
	}
}