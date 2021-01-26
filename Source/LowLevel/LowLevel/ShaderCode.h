// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/ShaderDigest.h"

namespace Flint
{
	enum class ShaderCodeType : UI8 {
		UNDEFINED,
		GLSL, SPIR_V, HLSL
	};

	/**
	 * Shader Code object.
	 * This objest stores shader code which will then be converted to shader modules by the backends.
	 */
	class ShaderCode {
	public:
		ShaderCode() {}
		~ShaderCode() {}

		/**
		 * Initialize the shader code by loading it to memory.
		 *
		 * @param pAsset: The asset path.
		 * @param location: The location of the shader in the pipeline.
		 * @param type: The shader code type.
		 */
		void Initialize(const char* pAsset, ShaderLocation location, ShaderCodeType type);

		/**
		 * Perform digestion and get the shader digest to create the shader module
		 */
		ShaderDigest Digest();

		/**
		 * Terminate the shader code by dumping the code.
		 */
		void Terminate();

	private:
		std::vector<UI32> mCode;
		ShaderCodeType mType = ShaderCodeType::UNDEFINED;
		ShaderLocation mLocation = ShaderLocation::UNDEFINED;
	};
}