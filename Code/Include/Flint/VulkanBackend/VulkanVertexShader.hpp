// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/VertexShader.hpp"
#include "VulkanShader.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanVertexShader final : public VertexShader
		{
		public:
			VulkanVertexShader(Device& device, const std::filesystem::path& path, ShaderCodeType type)
						: VertexShader(device, path, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, path, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}

			VulkanVertexShader(Device& device, const std::vector<UI32>& code, ShaderCodeType type = ShaderCodeType::SPIR_V)
						: VertexShader(device, code, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, code, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}

			VulkanVertexShader(Device& device, const std::string& code, ShaderCodeType type = ShaderCodeType::GLSL)
						: VertexShader(device, code, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, code, type)
		{
			ShaderDescriptor resource = std::move(vShader.PerformReflection());
			mResources = std::move(resource.mResources);
			mInputAttributes = std::move(resource.mInputAttributes);
			mOutputAttributes = std::move(resource.mOutputAttributes);
		}

			virtual void Terminate() override final
					{
			vShader.Terminate();
		}

			VulkanShader& GetVulkanShader() const { return const_cast<VulkanShader&>(vShader); }

		private:
			VulkanShader vShader;
		};
	}
}