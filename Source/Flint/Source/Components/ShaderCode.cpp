// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Components/ShaderCode.h"
#include "Core/ErrorHandler/Logger.h"

#include <fstream>

namespace Flint
{
	void ShaderCode::LoadFromFile(const char* pAsset, ShaderLocation location, ShaderCodeType type)
	{
		std::ifstream file(pAsset, std::ios::binary | std::ios::ate);

		if (!file.is_open())
		{
			FLINT_LOG_ERROR(TEXT("Failed to load the shader asset file! Provided location: #7"), pAsset)
				return;
		}

		UI64 size = file.tellg();
		file.seekg(0);

		mDigest.mCode.resize(size / sizeof(UI32));
		file.read(reinterpret_cast<char*>(mDigest.mCode.data()), size);

		file.close();
	}
}