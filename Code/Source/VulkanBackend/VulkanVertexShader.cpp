// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanVertexShader.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanVertexShader::VulkanVertexShader(Device& device, const std::filesystem::path& path, ShaderCodeType type)
			: VertexShader(device, path, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, path, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}

		VulkanVertexShader::VulkanVertexShader(Device& device, const std::vector<UI32>& code, ShaderCodeType type)
			: VertexShader(device, code, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, code, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}

		VulkanVertexShader::VulkanVertexShader(Device& device, const std::string& code, ShaderCodeType type)
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