// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Shader.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Tessellation Shader object.
		 */
		class TessallationShader : public Shader
		{
		public:
			TessallationShader(Device* pDevice, const std::filesystem::path& asset, const ShaderCodeType& codeType) : Shader(pDevice, asset, codeType, ShaderLocation::TESSELLATION) {}
		};
	}
}