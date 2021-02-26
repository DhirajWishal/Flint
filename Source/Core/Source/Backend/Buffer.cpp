// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/Buffer.h"
#include <algorithm>

namespace Flint
{
	namespace Backend
	{
		void Buffer::Reset()
		{
			UI8* pStore = static_cast<UI8*>(MapMemory(mSize, 0));
			std::fill(pStore, pStore + mSize, static_cast<UI8>(0));
			UnmapMemory();

			FlushMemoryMappings();
		}
	}
}