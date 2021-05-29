// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderTools/SPIRV/Utilities.h"

namespace Flint
{
	namespace ShaderTools
	{
		namespace Utilities
		{
			std::vector<UI32> ResolvePadding(const std::vector<UI32>& code)
			{
				// Get the final code size.
				const UI64 finalCodeSize = code.size() / 4;

				// Resize the vector.
				std::vector<UI32> resolvedCode(finalCodeSize);

				// Copy and return the copied vector.
				std::copy(code.begin(), code.begin() + finalCodeSize, resolvedCode.data());
				return resolvedCode;
			}
		}
	}
}