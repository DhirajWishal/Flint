// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Shader.hpp"

namespace Flint
{
	/**
	 * Shader code type enum.
	 */
	enum class ShaderCodeType : UI8 {
		GLSL,
		HLSL
	};

	/**
	 * Flint shader compiler.
	 * This compiler compiles the provided high level shader code to SPIR-V.
	 */
	class ShaderCompiler
	{
	public:
		/**
		 * Compile the shader by loading the code from a file.
		 * 
		 * @param shaderFile The shader file to compile.
		 * @param codeType The shader file type. Currently only supports GLSL.
		 * @param shaderType The type of the shader.
		 */
		ShaderCompiler(const std::filesystem::path& shaderFile, const ShaderCodeType codeType, const ShaderType shaderType);

		/**
		 * Compile the shader using a high level shader code.
		 *
		 * @param shaderCode The shader code to compile.
		 * @param codeType The shader file type. Currently only supports GLSL.
		 * @param shaderType The type of the shader.
		 */
		ShaderCompiler(const std::string& shaderCode, const ShaderCodeType codeType, const ShaderType shaderType);

		/**
		 * Get the compiled shader code.
		 * 
		 * @return The shader code vector.
		 */
		const std::vector<UI32> GetShaderCode() const { return mShaderCode; }

		/**
		 * Create the shader object using the shader code.
		 * 
		 * @param pDevice The device pointer.
		 * @return The shader pointer.
		 */
		std::shared_ptr<Shader> CreateShader(const std::shared_ptr<Device>& pDevice) const;

	private:
		std::vector<UI32> mShaderCode = {};
		ShaderType mType = ShaderType::Vertex;
		ShaderCodeType mCodeType = ShaderCodeType::GLSL;
	};
}
