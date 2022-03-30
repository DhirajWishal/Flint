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
			std::vector<VkDescriptorSetLayoutBinding> m_LayoutBindings;
			std::vector<VkDescriptorPoolSize> m_PoolSizes;
		};

		class VulkanShader final : public Shader<VulkanDevice>
		{
		public:
			explicit VulkanShader(VulkanDevice* pDevice, const ShaderType type, const std::filesystem::path& path);
			explicit VulkanShader(VulkanDevice* pDevice, const ShaderType type, const std::vector<uint32_t>& code);
			explicit VulkanShader(VulkanDevice* pDevice, const ShaderType type, const std::string& code);
			~VulkanShader() { if (!bIsTerminated) Terminate(); }

			virtual void CreateCache(const std::filesystem::path& path) const override;
			virtual void Reload(const std::filesystem::path& path) override;
			virtual void Reload(const std::vector<uint32_t>& code) override;
			virtual void Reload(const std::string& code) override;

			virtual void Terminate() override;

			VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;
			void PerformReflection();

			VkShaderModule GetModule() const { return vModule; }
			const std::vector<VkPushConstantRange> GetPushConstantRanges() const { return m_ConstantRanges; }
			const std::unordered_map<uint32_t, DescriptorSetInfo> GetDescriptorSetMap() const { return m_DescriptorSetMap; }

		private:
			void ResolveShaderStage();
			void CreateShaderModule();

		private:
			std::vector<uint32_t> m_ShaderCode;
			std::vector<VkPushConstantRange> m_ConstantRanges;

			std::unordered_map<uint32_t, DescriptorSetInfo> m_DescriptorSetMap;

			VkShaderModule vModule = VK_NULL_HANDLE;

			VkShaderStageFlags vStageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		};
	}
}