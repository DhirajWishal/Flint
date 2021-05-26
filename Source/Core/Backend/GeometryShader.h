// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Geometry Shader object.
		 */
		class GeometryShader : public Shader
		{
		public:
			GeometryShader(Device* pDevice, const std::filesystem::path& asset, const ShaderCodeType& codeType) : Shader(pDevice, asset, codeType, ShaderLocation::GEOMETRY) {}
		};
	}
}