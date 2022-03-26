// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Tools/SmartShaderCompiler.hpp"

namespace Flint
{
	void SmartShaderCompiler::AddInput(const InputAttributeType type, const uint32 location)
	{
		//mInputAttributes.push_back(std::pair{ type, location });
	}

	void SmartShaderCompiler::AddOutput(const InputAttributeType type, const uint32 location)
	{
		//mOutputAttributes.emplace_back(std::pair{ type, location });
	}

	void SmartShaderCompiler::AddUniform(const ShaderResourceType type, const uint32 setIndex, const uint32 binding)
	{
		//mResourceSetMap[setIndex].emplace_back(std::pair{ type, binding });
	}

	void SmartShaderCompiler::Compile()
	{
	}
}