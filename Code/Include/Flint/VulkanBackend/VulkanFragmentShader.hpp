// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/FragmentShader.hpp"
#include "VulkanShader.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanFragmentShader final : public FragmentShader
		{
		public:
			VulkanFragmentShader(Device &device, const std::filesystem::path &path, ShaderCodeType type)
				: FragmentShader(device, path, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, path, type)
			{
				ShaderDescriptor resource = std::move(vShader.PerformReflection());
				mResources = std::move(resource.mResources);
				mInputAttributes = std::move(resource.mInputAttributes);
				mOutputAttributes = std::move(resource.mOutputAttributes);
			}

			VulkanFragmentShader(Device &device, const std::vector<UI32> &code, ShaderCodeType type = ShaderCodeType::SPIR_V)
				: FragmentShader(device, code, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, code, type)
			{
				ShaderDescriptor resource = std::move(vShader.PerformReflection());
				mResources = std::move(resource.mResources);
				mInputAttributes = std::move(resource.mInputAttributes);
				mOutputAttributes = std::move(resource.mOutputAttributes);
			}

			VulkanFragmentShader(Device &device, const std::string &code, ShaderCodeType type = ShaderCodeType::GLSL)
				: FragmentShader(device, code, type), vShader(device.StaticCast<VulkanDevice>(), VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, code, type)
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

			VulkanShader &GetVulkanShader() const { return const_cast<VulkanShader &>(vShader); }

		private:
			VulkanShader vShader;
		};
	}
}