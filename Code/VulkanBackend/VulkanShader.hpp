// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanShader final : public Shader, public boost::enable_shared_from_this<VulkanShader>
		{
		public:
			VulkanShader(const boost::shared_ptr<Device>& pDevice, const ShaderType type, const boost::filesystem::path& path);
			VulkanShader(const boost::shared_ptr<Device>& pDevice, const ShaderType type, const boost::container::vector<UI32>& code);
			VulkanShader(const boost::shared_ptr<Device>& pDevice, const ShaderType type, const std::string& code);
			~VulkanShader() { if (!bIsTerminated) Terminate(); }

			virtual void Reload(const boost::filesystem::path& path) override final;
			virtual void Reload(const boost::container::vector<UI32>& code) override final;
			virtual void Reload(const std::string& code) override final;

			virtual void Terminate() override final;

			VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;
			void PerformReflection();

			VkShaderModule GetModule() const { return vModule; }
			const boost::container::vector<VkDescriptorSetLayoutBinding> GetResourceBindings() const { return mBindings; }
			const boost::container::vector<VkDescriptorPoolSize> GetPoolSizes() const { return mSizes; }
			const boost::container::vector<VkPushConstantRange> GetPushConstantRanges() const { return mConstantRanges; }

		private:
			void ResolveShaderStage();
			void CreateShaderModule();

		private:
			boost::container::vector<UI32> mShaderCode;
			boost::container::vector<VkDescriptorSetLayoutBinding> mBindings;
			boost::container::vector<VkDescriptorPoolSize> mSizes;
			boost::container::vector<VkPushConstantRange> mConstantRanges;

			VkShaderModule vModule = VK_NULL_HANDLE;

			VkShaderStageFlags vStageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		};
	}
}