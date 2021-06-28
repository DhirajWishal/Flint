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
			FragmentShader(Device& device, const std::filesystem::path& path, ShaderCodeType type) : Shader(device, path, type) {}
			FragmentShader(Device& device, const std::vector<UI32>& code, ShaderCodeType type = ShaderCodeType::SPIR_V) : Shader(device, code, type) {}
			FragmentShader(Device& device, const std::string& code, ShaderCodeType type = ShaderCodeType::GLSL) : Shader(device, code, type) {}
		};
	}
}