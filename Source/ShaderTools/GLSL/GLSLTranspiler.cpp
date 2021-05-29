// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderTools/GLSL/GLSLTranspiler.h"

#include <sstream>

#define SHADERC_SHAREDLIB
#include <shaderc\shaderc.hpp>

//#include <glslang\SPIRV\GlslangToSpv.h>

#include <iostream>

namespace Flint
{
	namespace ShaderTools
	{
		std::vector<UI32> GLSLTranspiler::GetHLSL()
		{
			return std::vector<UI32>();
		}

		std::vector<UI32> GLSLTranspiler::GetSPIRV()
		{
			std::string shaderCodeGLSL;
			shaderCodeGLSL.resize(pShaderCode->size() * sizeof(UI32));
			std::copy(reinterpret_cast<char*>(pShaderCode->data()), reinterpret_cast<char*>(pShaderCode->end()._Ptr), shaderCodeGLSL.data());

			//glslang::TIntermediate intermediate{ EShLanguage::EShLangVertex };
			//intermediate.addSourceText(shaderCodeGLSL.data(), shaderCodeGLSL.size());
			//
			//pShaderCode->clear();
			//glslang::GlslangToSpv(intermediate, *pShaderCode);

			//glslang::TShader shader{ EShLanguage::EShLangVertex };
			//shader.setStrings(strings.data(), strings.size());

			//std::string shaderCodeGLSL;
			//shaderCodeGLSL.resize(pShaderCode->size() * sizeof(UI32));
			//std::copy(reinterpret_cast<char*>(pShaderCode->data()), reinterpret_cast<char*>(pShaderCode->end()._Ptr), shaderCodeGLSL.data());
			//
			//shaderc::Compiler compiler = {};
			//auto result = compiler.CompileGlslToSpvAssembly(shaderCodeGLSL, shaderc_shader_kind::shaderc_glsl_infer_from_source, shaderCodeGLSL.data(), shaderc::CompileOptions());
			//
			//auto warnings = result.GetNumWarnings();
			//auto errors = result.GetNumErrors();
			//std::cout << result.GetErrorMessage();
			//auto stat = result.GetCompilationStatus();
			//
			//std::vector<UI32> code(std::distance(result.begin(), result.end()));
			//code.insert(code.end(), result.begin(), result.end());
			//
			//return code;
			return std::vector<UI32>();
		}
	}
}