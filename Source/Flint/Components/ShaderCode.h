// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/ShaderDigest.h"

namespace Flint
{
	enum class ShaderCodeType : UI8 {
		UNDEFINED,
		SPIR_V,
		GLSL,
		HLSL
	};

	/**
	 * Shader code object for the Flint engine.
	 * This object holds information about a single shader file/ code.
	 */
	class ShaderCode {
	public:
		ShaderCode() {}

		/**
		 * Load the shader code from an asset file.
		 * 
		 * @param pAsset: The asset to load the code from.
		 * @param location: The pipeline location to which the shader is used.
		 * @param type: The shader code type.
		 */
		void LoadFromFile(const char* pAsset, ShaderLocation location, ShaderCodeType type);

		ShaderCodeType mType = ShaderCodeType::UNDEFINED;
		ShaderDigest mDigest = {};
	};
}