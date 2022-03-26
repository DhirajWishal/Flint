// Copyright 2021-2022 Dhiraj Wishal
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
		uint64 HashDataBlock(const void* pDataBlock, const uint64 size, const uint64 seed = 0);
	}
}