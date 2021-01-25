// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Objects/ShaderCode.h"
#include "Core/ErrorHandler/Logger.h"

#include <fstream>

namespace Flint
{
	void ShaderCode::Initialize(const char* pAsset, ShaderLocation location, ShaderType type)
	{
		mLocation = location;
		mType = type;

		std::ifstream file(pAsset, std::ios::ate | std::ios::binary);

		// Check if the shader code is loaded.
		if (!file.is_open())
		{
			Logger::LogError(TEXT("Failed to load the provided shader file!"));
			return;
		}

		// Allocate the buffer and copy the content to it.
		I64 fileSize = static_cast<I64>(file.tellg());
		mCode.resize(fileSize);
		file.seekg(0);
		file.read(reinterpret_cast<char*>(mCode.data()), fileSize);
		file.close();
	}

	void ShaderCode::Reflect()
	{

	}
	
	void ShaderCode::Terminate()
	{
		mCode.clear();
	}
}