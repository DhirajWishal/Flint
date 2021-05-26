// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Fragment Shader object.
		 * This is the final shader program to be executes and performs coloring and texturing after rasterization.
		 */
		class FragmentShader : public Shader
		{
		public:
			FragmentShader(Device* pDevice, const std::filesystem::path& asset, const ShaderCodeType& codeType) : Shader(pDevice, asset, codeType, ShaderLocation::FRAGMENT) {}
		};
	}
}