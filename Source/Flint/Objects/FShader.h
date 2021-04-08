// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDevice.h"

#include <filesystem>

namespace Flint
{
	enum class ShaderCodeType : UI8 {
		UNDEFINED,
		SPIR_V,
		GLSL,
		HLSL
	};

	/**
	 * Flint shader object.
	 * This object holds information about a single shader object.
	 */
	class FShader final : public FObject {
	public:
		FShader();
		~FShader();

		/**
		 * Initialize the shader.
		 * This function initializes the shader by loading the asset data form a file.
		 *
		 * @param assetFile: The asset file to load the shader data from.
		 * @param location: The shader location.
		 * @param codeType: The shader code type.
		 * @param transpileTo: Whether or not to transpile the code to another type before initializing.
		 */
		void Initialize(const std::filesystem::path& assetFile, ShaderLocation location, ShaderCodeType codeType, ShaderCodeType transpileTo = ShaderCodeType::UNDEFINED);

		/**
		 * Terminate the shader.
		 */
		void Terminate();

		/**
		 * Get the shader digest.
		 * 
		 * @return The shader digest.
		 */
		ShaderDigest& GetShaderDigest() { return mDigest; }

		/**
		 * Get the shader digest.
		 *
		 * @return The shader digest.
		 */
		ShaderDigest const GetShaderDigest() const { return mDigest; }

	private:
		/**
		 * Load the shader data from a file.
		 * 
		 * @param assetFile: The file's path.
		 * @return Boolean value stating if successfully loaded or not.
		 */
		bool LoadShaderDataFromFile(const std::filesystem::path& assetFile);

		/**
		 * Transpile the shader code to a new type.
		 * 
		 * @param newType: The new type to be transpiled to.
		 */
		void Transpile(ShaderCodeType newType);

		/**
		 * Create the shader digest using the shader code.
		 * 
		 * @return The shader digest data.
		 */
		ShaderDigest CreateShaderDigest();

		ShaderDigest mDigest = {};
		ShaderCodeType mType = ShaderCodeType::UNDEFINED;
	};
}