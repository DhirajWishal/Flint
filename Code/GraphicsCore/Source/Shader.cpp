// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/Shader.hpp"

namespace Flint
{
	Shader::Shader(const boost::shared_ptr<Device>& pDevice, const ShaderType type, const boost::filesystem::path& path) : DeviceBoundObject(pDevice), mType(type)
	{
		if (type == ShaderType::UNDEFINED)
			FLINT_THROW_INVALID_ARGUMENT("Shader type should not be UNDEFINED!");

		if (path.empty())
			FLINT_THROW_INVALID_ARGUMENT("Shader file path should not be empty!");
	}

	Shader::Shader(const boost::shared_ptr<Device>& pDevice, const ShaderType type, const boost::container::vector<UI32>& code) : DeviceBoundObject(pDevice), mType(type)
	{
		if (type == ShaderType::UNDEFINED)
			FLINT_THROW_INVALID_ARGUMENT("Shader type should not be UNDEFINED!");

		if (code.empty())
			FLINT_THROW_INVALID_ARGUMENT("Shader code should not be empty!");
	}

	Shader::Shader(const boost::shared_ptr<Device>& pDevice, const ShaderType type, const std::string& code) : DeviceBoundObject(pDevice), mType(type)
	{
		if (type == ShaderType::UNDEFINED)
			FLINT_THROW_INVALID_ARGUMENT("Shader type should not be UNDEFINED!");

		if (code.empty())
			FLINT_THROW_INVALID_ARGUMENT("Shader code should not be empty!");
	}
}