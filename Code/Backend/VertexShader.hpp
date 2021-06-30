// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint vertex shader object.
		 * Vertex shaders are used in the graphics pipeline and is called per vertex.
		 */
		class VertexShader : public Shader
		{
		public:
			VertexShader(const std::shared_ptr<Device>& pDevice, const std::filesystem::path& path, ShaderCodeType type) : Shader(pDevice, ShaderType::VERTEX, path, type) {}
			VertexShader(const std::shared_ptr<Device>& pDevice, const std::vector<UI32>& code, ShaderCodeType type = ShaderCodeType::SPIR_V) : Shader(pDevice, ShaderType::VERTEX, code, type) {}
			VertexShader(const std::shared_ptr<Device>& pDevice, const std::string& code, ShaderCodeType type = ShaderCodeType::GLSL) : Shader(pDevice, ShaderType::VERTEX, code, type) {}
		};
	}
}