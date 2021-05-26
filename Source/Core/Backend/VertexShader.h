// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Vertex Shader object.
		 * This object stores information about a single vertex shader.
		 */
		class VertexShader : public Shader
		{
		public:
			VertexShader(Device* pDevice, const std::filesystem::path& asset, const ShaderCodeType& codeType) : Shader(pDevice, asset, codeType, ShaderLocation::VERTEX) {}
		};
	}
}