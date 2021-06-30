// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint fragment shader object.
		 * Fragment shaders are used to color pixels after rasterization.
		 */
		class FragmentShader : public Shader
		{
		public:
			FragmentShader(const std::shared_ptr<Device>& pDevice, const std::filesystem::path& path, ShaderCodeType type) : Shader(pDevice, ShaderType::FRAGMENT, path, type) {}
			FragmentShader(const std::shared_ptr<Device>& pDevice, const std::vector<UI32>& code, ShaderCodeType type = ShaderCodeType::SPIR_V) : Shader(pDevice, ShaderType::FRAGMENT, code, type) {}
			FragmentShader(const std::shared_ptr<Device>& pDevice, const std::string& code, ShaderCodeType type = ShaderCodeType::GLSL) : Shader(pDevice, ShaderType::FRAGMENT, code, type) {}
		};
	}
}