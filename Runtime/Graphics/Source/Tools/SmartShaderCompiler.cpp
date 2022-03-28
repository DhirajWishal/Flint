// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Tools/SmartShaderCompiler.hpp"

namespace Flint
{
	void SmartShaderCompiler::AddInput(const InputAttributeType type, const uint32_t location)
	{
		//m_InputAttributes.push_back(std::pair{ type, location });
	}

	void SmartShaderCompiler::AddOutput(const InputAttributeType type, const uint32_t location)
	{
		//m_OutputAttributes.emplace_back(std::pair{ type, location });
	}

	void SmartShaderCompiler::AddUniform(const ShaderResourceType type, const uint32_t setIndex, const uint32_t binding)
	{
		//m_ResourceSetMap[setIndex].emplace_back(std::pair{ type, binding });
	}

	void SmartShaderCompiler::Compile()
	{
	}
}