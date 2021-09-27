// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/SmartShaderCompiler.hpp"

namespace Flint
{
	void SmartShaderCompiler::AddInput(const InputAttributeType type, const UI32 location)
	{
		mInputAttributes.push_back({ type, location });
	}

	void SmartShaderCompiler::AddOutput(const InputAttributeType type, const UI32 location)
	{
		mOutputAttributes.push_back({ type, location });
	}

	void SmartShaderCompiler::AddUniform(const ShaderResourceType type, const UI32 setIndex, const UI32 binding)
	{
		mResourceSetMap[setIndex].push_back({ type, binding });
	}

	void SmartShaderCompiler::Compile()
	{
	}
}