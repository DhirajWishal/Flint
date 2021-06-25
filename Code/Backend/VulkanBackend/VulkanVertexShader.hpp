// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VertexShader.hpp"
#include "VulkanShader.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanVertexShader final : public VertexShader
		{
		public:
			VulkanVertexShader(Device& device, const std::filesystem::path& path, ShaderCodeType type);
			VulkanVertexShader(Device& device, const std::vector<UI32>& code, ShaderCodeType type = ShaderCodeType::SPIR_V);
			VulkanVertexShader(Device& device, const std::string& code, ShaderCodeType type = ShaderCodeType::GLSL);

			virtual void Terminate() override final;

			VulkanShader& GetVulkanShader() const { return const_cast<VulkanShader&>(vShader); }

		private:
			VulkanShader vShader;
		};
	}
}