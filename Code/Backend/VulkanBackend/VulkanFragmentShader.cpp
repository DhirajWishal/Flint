// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanFragmentShader.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanFragmentShader::VulkanFragmentShader(Backend::Device& device, const std::filesystem::path& path, Backend::ShaderCodeType type)
			: FragmentShader(device, path, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, path, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}

		VulkanFragmentShader::VulkanFragmentShader(Backend::Device& device, const std::vector<UI32>& code, Backend::ShaderCodeType type)
			: FragmentShader(device, code, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, code, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}

		VulkanFragmentShader::VulkanFragmentShader(Backend::Device& device, const std::string& code, Backend::ShaderCodeType type)
			: FragmentShader(device, code, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, code, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}

		void VulkanFragmentShader::Terminate()
		{
			vShader.Terminate();
		}
	}
}