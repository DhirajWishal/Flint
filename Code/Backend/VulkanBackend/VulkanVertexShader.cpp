// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanVertexShader.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanVertexShader::VulkanVertexShader(Backend::Device& device, const std::filesystem::path& path, Backend::ShaderCodeType type)
			: VertexShader(device, path, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, path, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}

		VulkanVertexShader::VulkanVertexShader(Backend::Device& device, const std::vector<UI32>& code, Backend::ShaderCodeType type)
			: VertexShader(device, code, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, code, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}

		VulkanVertexShader::VulkanVertexShader(Backend::Device& device, const std::string& code, Backend::ShaderCodeType type)
			: VertexShader(device, code, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, code, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}
		
		void VulkanVertexShader::Terminate()
		{
			vShader.Terminate();
		}
	}
}