// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/Shader.hpp"

namespace Flint
{
	Shader::Shader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::filesystem::path& path) : DeviceBoundObject(pDevice), mType(type)
	{
		if (type == ShaderType::Undefined)
			throw std::invalid_argument("Shader type should not be Undefined!");

		if (path.empty())
			throw std::invalid_argument("Shader file path should not be empty!");
	}

	Shader::Shader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::vector<UI32>& code) : DeviceBoundObject(pDevice), mType(type)
	{
		if (type == ShaderType::Undefined)
			throw std::invalid_argument("Shader type should not be Undefined!");

		if (code.empty())
			throw std::invalid_argument("Shader code should not be empty!");
	}

	Shader::Shader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::string& code) : DeviceBoundObject(pDevice), mType(type)
	{
		if (type == ShaderType::Undefined)
			throw std::invalid_argument("Shader type should not be Undefined!");

		if (code.empty())
			throw std::invalid_argument("Shader code should not be empty!");
	}
}