// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/ShaderCompiler.hpp"

#include <shaderc/shaderc.hpp>
#include <fstream>

#include <iostream>

namespace Flint
{
	namespace Helpers
	{
		shaderc_shader_kind GetShaderKindGLSL(const ShaderType type)
		{
			switch (type)
			{
			case Flint::ShaderType::VERTEX:
				return shaderc_shader_kind::shaderc_glsl_vertex_shader;

			case Flint::ShaderType::TESSELLATION_CONTROL:
				return shaderc_shader_kind::shaderc_glsl_tess_control_shader;

			case Flint::ShaderType::TESSELLATION_EVALUATION:
				return shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader;

			case Flint::ShaderType::GEOMETRY:
				return shaderc_shader_kind::shaderc_glsl_geometry_shader;

			case Flint::ShaderType::FRAGMENT:
				return shaderc_shader_kind::shaderc_glsl_fragment_shader;

			case Flint::ShaderType::COMPUTE:
				return shaderc_shader_kind::shaderc_glsl_compute_shader;

			case Flint::ShaderType::RAY_GEN:
				return shaderc_shader_kind::shaderc_glsl_raygen_shader;

			case Flint::ShaderType::ANY_HIT:
				return shaderc_shader_kind::shaderc_glsl_anyhit_shader;

			case Flint::ShaderType::CLOSEST_HIT:
				return shaderc_shader_kind::shaderc_glsl_closesthit_shader;

			case Flint::ShaderType::RAY_MISS:
				return shaderc_shader_kind::shaderc_glsl_miss_shader;

			default:
				FLINT_THROW_RUNTIME_ERROR("Invalid or undefined shader type!");
			}

			return shaderc_shader_kind::shaderc_glsl_default_vertex_shader;
		}

		shaderc_shader_kind GetShaderKindHLSL(const ShaderType type)
		{
			switch (type)
			{
			case Flint::ShaderType::VERTEX:
				return shaderc_shader_kind::shaderc_vertex_shader;

			case Flint::ShaderType::TESSELLATION_CONTROL:
				return shaderc_shader_kind::shaderc_tess_control_shader;

			case Flint::ShaderType::TESSELLATION_EVALUATION:
				return shaderc_shader_kind::shaderc_tess_evaluation_shader;

			case Flint::ShaderType::GEOMETRY:
				return shaderc_shader_kind::shaderc_geometry_shader;

			case Flint::ShaderType::FRAGMENT:
				return shaderc_shader_kind::shaderc_fragment_shader;

			case Flint::ShaderType::COMPUTE:
				return shaderc_shader_kind::shaderc_compute_shader;

			case Flint::ShaderType::RAY_GEN:
				return shaderc_shader_kind::shaderc_raygen_shader;

			case Flint::ShaderType::ANY_HIT:
				return shaderc_shader_kind::shaderc_anyhit_shader;

			case Flint::ShaderType::CLOSEST_HIT:
				return shaderc_shader_kind::shaderc_closesthit_shader;

			case Flint::ShaderType::RAY_MISS:
				return shaderc_shader_kind::shaderc_miss_shader;

			default:
				FLINT_THROW_RUNTIME_ERROR("Invalid or undefined shader type!");
			}

			return shaderc_shader_kind::shaderc_vertex_shader;
		}
	}

	ShaderCompiler::ShaderCompiler(const std::filesystem::path& shaderFile, const ShaderCodeType codeType, const ShaderType shaderType)
		: mCodeType(codeType)
	{
		// Load the shade file.
		std::ifstream file(shaderFile, std::ios::in | std::ios::binary | std::ios::ate);
		UI64 size = file.tellg();
		file.seekg(0);

		// Get the shader code.
		std::string shaderCode;
		shaderCode.resize(size);
		file.read(shaderCode.data(), size);
		file.close();

		shaderc::Compiler compiler;
		shaderc::SpvCompilationResult result;
		shaderc::CompileOptions options;
		//options.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);

		// Compile the shader.
		if (codeType == ShaderCodeType::GLSL)
			result = compiler.CompileGlslToSpv(shaderCode, Helpers::GetShaderKindGLSL(shaderType), "", options);
		//else if (codeType == ShaderCodeType::HLSL)
		//	result = compiler.CompileGlslToSpv(shaderCode, Helpers::GetShaderKindHLSL(shaderType), shaderFile.filename().string().c_str());
		else
			FLINT_THROW_RUNTIME_ERROR("Invalid shader file type!");

		// Check the compilation result.
		if (result.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success)
			FLINT_THROW_RANGE_ERROR("Shader compilation failed!");

		// Insert the compiled shader code.
		mShaderCode.resize(std::distance(result.begin(), result.end()) * sizeof(UI32));
		std::copy(reinterpret_cast<const unsigned char*>(result.begin()), reinterpret_cast<const unsigned char*>(result.end()), reinterpret_cast<unsigned char*>(mShaderCode.data()));
	}

	ShaderCompiler::ShaderCompiler(const std::string& shaderCode, const ShaderCodeType codeType, const ShaderType shaderType)
		: mCodeType(codeType)
	{
		shaderc::Compiler compiler;
		shaderc::SpvCompilationResult result;
		shaderc::CompileOptions options;
		//options.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);

		// Compile the shader.
		if (codeType == ShaderCodeType::GLSL)
			result = compiler.CompileGlslToSpv(shaderCode, Helpers::GetShaderKindGLSL(shaderType), "", options);
		//else if (codeType == ShaderCodeType::HLSL)
		//	result = compiler.CompileGlslToSpv(shaderCode, Helpers::GetShaderKindHLSL(shaderType), shaderFile.filename().string().c_str());
		else
			FLINT_THROW_RUNTIME_ERROR("Invalid shader file type!");

		// Check the compilation result.
		if (result.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success)
			FLINT_THROW_RANGE_ERROR("Shader compilation failed!");

		// Insert the compiled shader code.
		mShaderCode.resize(std::distance(result.begin(), result.end()) * sizeof(UI32));
		std::copy(reinterpret_cast<const unsigned char*>(result.begin()), reinterpret_cast<const unsigned char*>(result.end()), reinterpret_cast<unsigned char*>(mShaderCode.data()));
	}
}