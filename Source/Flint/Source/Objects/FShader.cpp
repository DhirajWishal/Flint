// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FShader.h"
#include "Core\ErrorHandler\Logger.h"

#include "ShaderTools\SPIRV\Reflection.h"
#include "ShaderTools\SPIRV\SPIRVTranspiler.h"
#include "ShaderTools\GLSL\GLSLTranspiler.h"

#include <fstream>


namespace Flint
{
	FShader::FShader()
	{
	}

	FShader::~FShader()
	{
	}

	void FShader::Initialize(const std::filesystem::path& assetFile, ShaderLocation location, ShaderCodeType codeType, ShaderCodeType transpileTo)
	{
		mType = codeType;
		mDigest.mLocation = location;
		if (!LoadShaderDataFromFile(assetFile))
			return;

		Transpile(transpileTo);
		mDigest = CreateShaderDigest();
	}

	void FShader::Terminate()
	{
		mDigest.mCode.clear();
		mDigest.mInputAttributes.clear();
		mDigest.mOutputAttributes.clear();
		mDigest.mUniforms.clear();
	}

	bool FShader::LoadShaderDataFromFile(const std::filesystem::path& assetFile)
	{
		std::ifstream file(assetFile, std::ios::binary | std::ios::ate);

		if (!file.is_open())
		{
			FLINT_LOG_ERROR(TEXT("Failed to load the shader asset file! Provided location: #7"), assetFile.string().c_str())
				return false;
		}

		UI64 size = file.tellg();
		file.seekg(0);

		mDigest.mCode.resize(size);
		file.read(reinterpret_cast<char*>(mDigest.mCode.data()), size);

		file.close();

		return true;
	}

	void FShader::Transpile(ShaderCodeType newType)
	{
		if (newType == ShaderCodeType::UNDEFINED)
			return;

		ShaderTools::Transpiler* pTranspiler = nullptr;

		switch (mType)
		{
		case Flint::ShaderCodeType::SPIR_V:
			pTranspiler = new ShaderTools::SPIRVTranspiler();
			break;

		case Flint::ShaderCodeType::GLSL:
			pTranspiler = new ShaderTools::GLSLTranspiler();
			break;

		case Flint::ShaderCodeType::HLSL:
			break;

		default:
			break;
		}

		pTranspiler->Initialize(&mDigest.mCode);
		switch (newType)
		{
		case Flint::ShaderCodeType::SPIR_V:
			mDigest.mCode = std::move(pTranspiler->GetSPIRV()), mType = newType;
			break;

		case Flint::ShaderCodeType::GLSL:
			mDigest.mCode = std::move(pTranspiler->GetGLSL()), mType = newType;
			break;

		case Flint::ShaderCodeType::HLSL:
			mDigest.mCode = std::move(pTranspiler->GetHLSL()), mType = newType;
			break;

		default:
			FLINT_LOG_ERROR(TEXT("Invalid or Undefined shader code types submitted to be transpiled!"))
				break;
		}

		delete pTranspiler;
	}

	ShaderDigest FShader::CreateShaderDigest()
	{
		switch (mType)
		{
		case Flint::ShaderCodeType::SPIR_V:
		{
			ShaderTools::SPIRVReflection reflection = {};
			return reflection.Reflect(mDigest.mCode, mDigest.mLocation);
		}
		break;

		case Flint::ShaderCodeType::GLSL:
			break;

		case Flint::ShaderCodeType::HLSL:
			break;

		default:
			FLINT_LOG_ERROR(TEXT("Invalid or Undefined shader code type!"))
				break;
		}

		return ShaderDigest();
	}
}