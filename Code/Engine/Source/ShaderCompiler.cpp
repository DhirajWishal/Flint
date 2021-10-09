// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/ShaderCompiler.hpp"

#include "GraphicsCore/Device.hpp"

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
			case Flint::ShaderType::Vertex:
				return shaderc_shader_kind::shaderc_glsl_vertex_shader;

			case Flint::ShaderType::TessellationControl:
				return shaderc_shader_kind::shaderc_glsl_tess_control_shader;

			case Flint::ShaderType::TessellationEvaluation:
				return shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader;

			case Flint::ShaderType::Geometry:
				return shaderc_shader_kind::shaderc_glsl_geometry_shader;

			case Flint::ShaderType::Fragment:
				return shaderc_shader_kind::shaderc_glsl_fragment_shader;

			case Flint::ShaderType::Compute:
				return shaderc_shader_kind::shaderc_glsl_compute_shader;

			case Flint::ShaderType::RayGen:
				return shaderc_shader_kind::shaderc_glsl_raygen_shader;

			case Flint::ShaderType::AnyHit:
				return shaderc_shader_kind::shaderc_glsl_anyhit_shader;

			case Flint::ShaderType::ClosestHit:
				return shaderc_shader_kind::shaderc_glsl_closesthit_shader;

			case Flint::ShaderType::RayMiss:
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
			case Flint::ShaderType::Vertex:
				return shaderc_shader_kind::shaderc_vertex_shader;

			case Flint::ShaderType::TessellationControl:
				return shaderc_shader_kind::shaderc_tess_control_shader;

			case Flint::ShaderType::TessellationEvaluation:
				return shaderc_shader_kind::shaderc_tess_evaluation_shader;

			case Flint::ShaderType::Geometry:
				return shaderc_shader_kind::shaderc_geometry_shader;

			case Flint::ShaderType::Fragment:
				return shaderc_shader_kind::shaderc_fragment_shader;

			case Flint::ShaderType::Compute:
				return shaderc_shader_kind::shaderc_compute_shader;

			case Flint::ShaderType::RayGen:
				return shaderc_shader_kind::shaderc_raygen_shader;

			case Flint::ShaderType::AnyHit:
				return shaderc_shader_kind::shaderc_anyhit_shader;

			case Flint::ShaderType::ClosestHit:
				return shaderc_shader_kind::shaderc_closesthit_shader;

			case Flint::ShaderType::RayMiss:
				return shaderc_shader_kind::shaderc_miss_shader;

			default:
				FLINT_THROW_RUNTIME_ERROR("Invalid or undefined shader type!");
			}

			return shaderc_shader_kind::shaderc_vertex_shader;
		}
	}

	ShaderCompiler::ShaderCompiler(const std::filesystem::path& shaderFile, const ShaderCodeType codeType, const ShaderType shaderType)
		: mCodeType(codeType), mType(shaderType)
	{
		// Load the shade file.
		std::ifstream file(shaderFile, std::ios::in | std::ios::binary | std::ios::ate);

		if (!file.is_open())
			throw std::invalid_argument("Provided asset file is not available!");

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
		options.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);

		// Compile the shader.
		if (codeType == ShaderCodeType::GLSL)
			result = compiler.CompileGlslToSpv(shaderCode, Helpers::GetShaderKindGLSL(shaderType), shaderFile.string().c_str(), options);
		//else if (codeType == ShaderCodeType::HLSL)
		//	result = compiler.CompileGlslToSpv(shaderCode, Helpers::GetShaderKindHLSL(shaderType), shaderFile.filename().string().c_str());
		else
			FLINT_THROW_RUNTIME_ERROR("Invalid shader file type!");

		// Check the compilation result.
		if (result.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success)
		{
			std::cerr << result.GetErrorMessage() << std::endl;
			throw std::range_error("Shader compilation failed!");
		}

		// Insert the compiled shader code.
		mShaderCode.resize(std::distance(result.begin(), result.end()) * sizeof(UI32));
		std::copy(reinterpret_cast<const unsigned char*>(result.begin()), reinterpret_cast<const unsigned char*>(result.end()), reinterpret_cast<unsigned char*>(mShaderCode.data()));
	}

	ShaderCompiler::ShaderCompiler(const std::string& shaderCode, const ShaderCodeType codeType, const ShaderType shaderType)
		: mCodeType(codeType), mType(shaderType)
	{
		shaderc::Compiler compiler;
		shaderc::SpvCompilationResult result;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);

		// Compile the shader.
		if (codeType == ShaderCodeType::GLSL)
			result = compiler.CompileGlslToSpv(shaderCode, Helpers::GetShaderKindGLSL(shaderType), "", options);
		//else if (codeType == ShaderCodeType::HLSL)
		//	result = compiler.CompileGlslToSpv(shaderCode, Helpers::GetShaderKindHLSL(shaderType), shaderFile.filename().string().c_str());
		else
			FLINT_THROW_RUNTIME_ERROR("Invalid shader file type!");

		// Check the compilation result.
		if (result.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success)
		{
			std::cerr << result.GetErrorMessage() << std::endl;
			throw std::range_error("Shader compilation failed!");
		}

		// Insert the compiled shader code.
		mShaderCode.resize(std::distance(result.begin(), result.end()) * sizeof(UI32));
		std::copy(reinterpret_cast<const unsigned char*>(result.begin()), reinterpret_cast<const unsigned char*>(result.end()), reinterpret_cast<unsigned char*>(mShaderCode.data()));
	}

	std::shared_ptr<Shader> ShaderCompiler::CreateShader(const std::shared_ptr<Device>& pDevice) const
	{
		return pDevice->CreateShader(mType, GetShaderCode());
	}
}