// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core\Backend\Shader.h"
#include "Core\ErrorHandler\Logger.h"

#include <fstream>

namespace Flint
{
	namespace Backend
	{
		Shader::Shader(Device* pDevice, const std::filesystem::path& asset, const ShaderCodeType& codeType, const ShaderLocation& location)
			: DeviceBoundObject(pDevice), mLocation(location), mCodeType(codeType)
		{
			std::fstream shaderFile(asset, std::ios::binary | std::ios::ate);

			if (!shaderFile.is_open())
			{
				Logger::LogError(TEXT("Submitted asset file could not be loaded by the shader!"));
				return;
			}

			UI64 codeSize = shaderFile.tellg();
			shaderFile.seekg(0);

			mShaderCode.resize(codeSize);
			shaderFile.read(reinterpret_cast<char*>(mShaderCode.data()), codeSize);
			shaderFile.close();
		}

		void Shader::PerformReflection()
		{
		}
	}
}