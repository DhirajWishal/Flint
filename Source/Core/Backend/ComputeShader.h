// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Compute Shader object.
		 */
		class ComputeShader : public Shader
		{
		public:
			ComputeShader(Device* pDevice, const std::filesystem::path& asset, const ShaderCodeType& codeType) : Shader(pDevice, asset, codeType, ShaderLocation::COMPUTE) {}
		};
	}
}
