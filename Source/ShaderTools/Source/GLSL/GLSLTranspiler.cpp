// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderTools/GLSL/GLSLTranspiler.h"

#include <glslang/Public/ShaderLang.h>

namespace Flint
{
	namespace ShaderTools
	{
		std::vector<UI32> GLSLTranspiler::GetHLSL()
		{
			glslang::InitializeProcess();
			glslang::TShader shader(EShLanguage::EShLangVertex);

			String shaderCode;
			shaderCode.resize((pShaderCode->size() * sizeof(UI32)) / sizeof(char));
			std::copy(pShaderCode->begin(), pShaderCode->end(), reinterpret_cast<UI32*>(shaderCode.data()));

			std::vector<const char*> pShaders = { shaderCode.c_str() };
			shader.setStrings(pShaders.data(), 1);

			glslang::FinalizeProcess();
			return std::vector<UI32>();
		}

		std::vector<UI32> GLSLTranspiler::GetSPIRV()
		{
			return std::vector<UI32>();
		}
	}
}