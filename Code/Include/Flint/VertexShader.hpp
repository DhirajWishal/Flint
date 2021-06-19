// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"

namespace Flint
{
	/**
	 * Flint vertex shader object.
	 * Vertex shaders are used in the graphics pipeline and is called per vertex.
	 */
	class VertexShader : public Shader
	{
	public:
		VertexShader(Device& device, const std::filesystem::path& path, ShaderCodeType type) : Shader(device, path, type) {}
		VertexShader(Device& device, const std::vector<UI32>& code, ShaderCodeType type = ShaderCodeType::SPIR_V) : Shader(device, code, type) {}
		VertexShader(Device& device, const std::string& code, ShaderCodeType type = ShaderCodeType::GLSL) : Shader(device, code, type) {}
	};
}