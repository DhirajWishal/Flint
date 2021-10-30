// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/SmartShaderCompiler.hpp"

namespace Flint
{
	void SmartShaderCompiler::AddInput(const InputAttributeType type, const uint32 location)
	{
		mInputAttributes.push_back({ type, location });
	}

	void SmartShaderCompiler::AddOutput(const InputAttributeType type, const uint32 location)
	{
		mOutputAttributes.push_back({ type, location });
	}

	void SmartShaderCompiler::AddUniform(const ShaderResourceType type, const uint32 setIndex, const uint32 binding)
	{
		mResourceSetMap[setIndex].push_back({ type, binding });
	}

	void SmartShaderCompiler::Compile()
	{
	}
}