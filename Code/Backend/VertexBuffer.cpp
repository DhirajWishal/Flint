// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VertexBuffer.hpp"

namespace Flint
{
	UI64 VertexDescriptor::Stride() const
	{
		UI64 size = 0;
		for (auto attribute : mAttributeTypes)
			size += static_cast<UI8>(attribute);

		return size;
	}
}