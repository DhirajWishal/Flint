// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FragmentShader.hpp"
#include "VulkanShader.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanFragmentShader final : public Backend::FragmentShader
		{
		public:
			VulkanFragmentShader(Backend::Device& device, const std::filesystem::path& path, Backend::ShaderCodeType type);
			VulkanFragmentShader(Backend::Device& device, const std::vector<UI32>& code, Backend::ShaderCodeType type = Backend::ShaderCodeType::SPIR_V);
			VulkanFragmentShader(Backend::Device& device, const std::string& code, Backend::ShaderCodeType type = Backend::ShaderCodeType::GLSL);

			virtual void Terminate() override final;

			VulkanShader& GetVulkanShader() const { return const_cast<VulkanShader&>(vShader); }

		private:
			VulkanShader vShader;
		};
	}
}