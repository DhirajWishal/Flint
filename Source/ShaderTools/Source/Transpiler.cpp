// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderTools/Transpiler.h"

namespace Flint
{
	namespace ShaderTools
	{
		void Transpiler::Initialize(std::vector<UI32>* pShaderCode)
		{
			this->pShaderCode = pShaderCode;
		}
	}
}