// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/ShaderCode.hpp"
#include "Core/Errors/AssetError.hpp"

#include <fstream>

namespace Flint
{

	ShaderCode::ShaderCode(std::vector<uint32_t>&& code)
		: m_Code(std::move(code))
	{

	}

	ShaderCode::ShaderCode(std::filesystem::path&& file)
	{
		// Load the shader code and perform reflection over it.
		auto shaderSource = std::fstream(file, std::ios::binary | std::ios::in | std::ios::ate);

		// Validate the shader file.
		if (!shaderSource.is_open())
			throw AssetError("Failed to load the shader file!");

		const auto shaderSize = shaderSource.tellg();
		shaderSource.seekg(0);

		// Load the shader's content.
		m_Code.resize(shaderSize);
		shaderSource.read(reinterpret_cast<char*>(m_Code.data()), shaderSize);
		shaderSource.close();
	}
}