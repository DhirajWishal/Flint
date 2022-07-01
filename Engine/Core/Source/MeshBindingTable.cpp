// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/MeshBindingTable.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

namespace Flint
{

	void MeshBindingTable::bind(uint32_t binding, const std::shared_ptr<Buffer>& pBuffer)
	{
		m_pBuffers[binding] = pBuffer;
	}

	uint64_t MeshBindingTable::generateHash() const
	{
		std::vector<uint64_t> hashes;
		hashes.reserve(m_pBuffers.size() * 2);
		for (const auto& [index, pBuffer] : m_pBuffers)
		{
			hashes.emplace_back(index);
			hashes.emplace_back(reinterpret_cast<uint64_t>(pBuffer.get()));
		}

		return static_cast<uint64_t>(XXH64(hashes.data(), sizeof(uint64_t) * hashes.size(), 0));
	}
}