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
			VulkanFragmentShader(Device& device, const std::filesystem::path& path, ShaderCodeType type);
			VulkanFragmentShader(Device& device, const std::vector<UI32>& code, ShaderCodeType type = ShaderCodeType::SPIR_V);
			VulkanFragmentShader(Device& device, const std::string& code, ShaderCodeType type = ShaderCodeType::GLSL);

			virtual void Terminate() override final;

			VulkanShader& GetVulkanShader() const { return const_cast<VulkanShader&>(vShader); }

		private:
			VulkanShader vShader;
		};
	}
}