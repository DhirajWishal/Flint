// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderTools/SPIRV/Transpiler.h"
#include "ShaderTools/SPIRV/Utilities.h"
#include "Core/ErrorHandler/Logger.h"

#include <spirv_glsl.hpp>
#include <spirv_hlsl.hpp>

namespace Flint
{
	namespace ShaderTools
	{
		void SPIRVTranspiler::Initialize(std::vector<UI32>* pShaderCode)
		{
			this->pShaderCode = pShaderCode;
		}

		std::vector<UI32> SPIRVTranspiler::GetGLSL()
		{
			// Create and initialize the shader code.
			std::vector<UI32> mShaderCode;

			// Check if the shader code object is set.
			if (!pShaderCode)
			{
				FLINT_LOG_ERROR(TEXT("Shader code is not set! Make sure to initialize the SPIR-V Transpiler with the shader code pointer."));
				return mShaderCode;
			}

			// Create the compiler.
			spirv_cross::CompilerGLSL mCompiler(std::move(Utilities::ResolvePadding(*pShaderCode)));

			// Compile to GLSL and get the byte size of it.
			auto mGLSl = mCompiler.compile();
			UI64 mSize = mGLSl.size() * sizeof(char);

			// Resize the vector.
			mShaderCode.resize(mSize / sizeof(UI32));

			// Move data from the string to the shader code and return it.
			std::move(mGLSl.begin(), mGLSl.end(), mShaderCode.data());
			return mShaderCode;
		}

		std::vector<UI32> SPIRVTranspiler::GetHLSL()
		{
			// Create and initialize the shader code.
			std::vector<UI32> mShaderCode = {};

			// Check if the shader code object is set.
			if (!pShaderCode)
			{
				FLINT_LOG_ERROR(TEXT("Shader code is not set! Make sure to initialize the SPIR-V Transpiler with the shader code pointer."));
				return mShaderCode;
			}

			// Create the compiler.
			spirv_cross::CompilerHLSL mCompiler(std::move(Utilities::ResolvePadding(*pShaderCode)));

			// Compile to GLSL and get the byte size of it.
			auto mHLSL = mCompiler.compile();
			UI64 mSize = mHLSL.size() * sizeof(char);

			// Resize the vector.
			mShaderCode.resize(mSize / sizeof(UI32));

			// Move data from the string to the shader code and return it.
			std::move(mHLSL.begin(), mHLSL.end(), mShaderCode.data());
			return mShaderCode;
		}
	}
}