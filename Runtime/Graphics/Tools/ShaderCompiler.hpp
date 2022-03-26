// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Shader.hpp"

namespace Flint
{
	/**
	 * Shader code type enum.
	 */
	enum class ShaderCodeType : uint8 {
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
#if defined FLINT_PLATFORM_WINDOWS
		/**
		 * Compile the shader by loading the code from a file.
		 *
		 * @param shaderFile The shader file to compile.
		 * @param codeType The shader file type. Currently only supports GLSL.
		 * @param shaderType The type of the shader.
		 * @param compilerTool The compiler tool path. Default is "Tools/Windows/glslangValidator".
		 */
		ShaderCompiler(const std::filesystem::path& shaderFile, const ShaderCodeType codeType, const ShaderType shaderType, std::filesystem::path compilerTool = "Tools/Windows/glslangValidator");

		/**
		 * Compile the shader using a high level shader code.
		 *
		 * @param shaderCode The shader code to compile.
		 * @param codeType The shader file type. Currently only supports GLSL.
		 * @param shaderType The type of the shader.
		 * @param compilerTool The compiler tool path. Default is "Tools/Windows/glslangValidator".
		 */
		ShaderCompiler(const std::string& shaderCode, const ShaderCodeType codeType, const ShaderType shaderType, std::filesystem::path compilerTool = "Tools/Windows/glslangValidator");

#elif defined FLINT_PLATFORM_LINUX
		/**
		 * Compile the shader by loading the code from a file.
		 *
		 * @param shaderFile The shader file to compile.
		 * @param codeType The shader file type. Currently only supports GLSL.
		 * @param shaderType The type of the shader.
		 * @param compilerTool The compiler tool path. Default is "Tools/Linux/glslangValidator".
		 */
		ShaderCompiler(const std::filesystem::path& shaderFile, const ShaderCodeType codeType, const ShaderType shaderType, std::filesystem::path compilerTool = "Tools/Linux/glslangValidator");

		/**
		 * Compile the shader using a high level shader code.
		 *
		 * @param shaderCode The shader code to compile.
		 * @param codeType The shader file type. Currently only supports GLSL.
		 * @param shaderType The type of the shader.
		 * @param compilerTool The compiler tool path. Default is "Tools/Linux/glslangValidator".
		 */
		ShaderCompiler(const std::string& shaderCode, const ShaderCodeType codeType, const ShaderType shaderType, std::filesystem::path compilerTool = "Tools/Linux/glslangValidator");

#endif // defined FLINT_PLATFORM_WINDOWS

		/**
		 * Get the compiled shader code.
		 *
		 * @return The shader code vector.
		 */
		const std::vector<uint32> GetShaderCode() const { return mShaderCode; }

		/**
		 * Create the shader object using the shader code.
		 *
		 * @param pDevice The device pointer.
		 * @return The shader pointer.
		 */
		std::shared_ptr<Shader> CreateShader(const std::shared_ptr<Device>& pDevice) const;

	private:
		std::vector<uint32> mShaderCode = {};
		ShaderType mType = ShaderType::Vertex;
		ShaderCodeType mCodeType = ShaderCodeType::GLSL;
	};
}
