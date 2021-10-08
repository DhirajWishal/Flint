// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DataType.hpp"

namespace Flint
{
	namespace Hasher
	{
		/**
		 * Perform hash to a block of data.
		 * 
		 * @param pDataBlock The data block pointer.
		 * @param size The size of the block in bytes.
		 * @param seed The hash seed.
		 * @return The hashed value.
		 */
		UI64 HashDataBlock(const void* pDataBlock, const UI64 size, const UI64 seed = 0);
	}
}