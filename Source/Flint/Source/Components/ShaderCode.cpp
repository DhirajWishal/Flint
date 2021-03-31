// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Components/ShaderCode.h"
#include "Core/ErrorHandler/Logger.h"

#include "ShaderTools/SPIRV/Reflection.h"
#include "ShaderTools/SPIRV/SPIRVTranspiler.h"
#include "ShaderTools/GLSL/GLSLTranspiler.h"

#include <fstream>

namespace Flint
{
	void ShaderCode::LoadFromFile(const char* pAsset, ShaderLocation location, ShaderCodeType type)
	{
		this->mType = type;
		this->mLocation = location;

		std::ifstream file(pAsset, std::ios::binary | std::ios::ate);

		if (!file.is_open())
		{
			FLINT_LOG_ERROR(TEXT("Failed to load the shader asset file! Provided location: #7"), pAsset)
				return;
		}

		UI64 size = file.tellg();
		file.seekg(0);

		mCode.resize(size);
		file.read(reinterpret_cast<char*>(mCode.data()), size);

		file.close();
	}

	void ShaderCode::Transpile(ShaderCodeType newType)
	{
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

		pTranspiler->Initialize(&mCode);
		switch (newType)
		{
		case Flint::ShaderCodeType::SPIR_V:
			mCode = std::move(pTranspiler->GetSPIRV()), mType = newType;
			break;

		case Flint::ShaderCodeType::GLSL:
			mCode = std::move(pTranspiler->GetGLSL()), mType = newType;
			break;

		case Flint::ShaderCodeType::HLSL:
			mCode = std::move(pTranspiler->GetHLSL()), mType = newType;
			break;

		default:
			FLINT_LOG_ERROR(TEXT("Invalid or Undefined shader code types submitted to be transpiled!"))
				break;
		}

		delete pTranspiler;
	}

	ShaderDigest ShaderCode::CreateDigest(ShaderCodeType newType)
	{
		if (newType != ShaderCodeType::UNDEFINED && newType != mType)
			Transpile(newType);

		switch (mType)
		{
		case Flint::ShaderCodeType::SPIR_V:
		{
			ShaderTools::SPIRVReflection reflection = {};
			return reflection.Reflect(mCode, mLocation);
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