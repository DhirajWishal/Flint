// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Shader.hpp"

namespace Flint
{
	Shader::Shader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::filesystem::path& path, ShaderCodeType codeType) : DeviceBoundObject(pDevice), mType(type)
	{
		if (type == ShaderType::UNDEFINED)
			FLINT_THROW_INVALID_ARGUMENT("Shader type should not be UNDEFINED!");

		if (path.empty())
			FLINT_THROW_INVALID_ARGUMENT("Shader file path should not be empty!");
	}

	Shader::Shader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::vector<UI32>& code, ShaderCodeType codeType) : DeviceBoundObject(pDevice), mType(type)
	{
		if (type == ShaderType::UNDEFINED)
			FLINT_THROW_INVALID_ARGUMENT("Shader type should not be UNDEFINED!");

		if (code.empty())
			FLINT_THROW_INVALID_ARGUMENT("Shader code should not be empty!");
	}

	Shader::Shader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::string& code, ShaderCodeType codeType) : DeviceBoundObject(pDevice), mType(type)
	{
		if (type == ShaderType::UNDEFINED)
			FLINT_THROW_INVALID_ARGUMENT("Shader type should not be UNDEFINED!");

		if (code.empty())
			FLINT_THROW_INVALID_ARGUMENT("Shader code should not be empty!");
	}
}