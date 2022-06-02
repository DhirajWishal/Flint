// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Utility/Hasher.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

namespace Flint
{
	uint64_t GenerateHash(const ResourceBindingTable& table)
	{
		// Generate the hashes for the bindings.
		std::vector<XXH64_hash_t> inputBindingHash;
		inputBindingHash.reserve(table.m_Images.size() + table.m_Buffers.size());

		for (const auto& [binding, images] : table.m_Images)
			inputBindingHash.emplace_back(XXH64(images.data(), sizeof(ImageHandle) * images.size(), 0));

		for (const auto& [binding, buffers] : table.m_Buffers)
			inputBindingHash.emplace_back(XXH64(buffers.data(), sizeof(BufferHandle) * buffers.size(), 0));

		// Finally combine all of them.
		return static_cast<uint64_t>(XXH64(inputBindingHash.data(), sizeof(XXH64_hash_t) * inputBindingHash.size(), 0));
	}

	uint64_t GenerateHash(const RasterizingPipelineSpecification& specification)
	{
		using PathCharacterType = std::filesystem::path::string_type::value_type;

		// Generate the hash for the whole structure.
		const XXH64_hash_t hashes[] = {
			XXH64(specification.m_CacheFile.native().data(), sizeof(PathCharacterType) * specification.m_CacheFile.native().size(), 0),
			XXH64(specification.m_VertexShader.getCode().data(), sizeof(uint32_t) * specification.m_VertexShader.getCode().size(), 0),
			XXH64(specification.m_FragmentShader.getCode().data(), sizeof(uint32_t) * specification.m_FragmentShader.getCode().size(), 0),
			XXH64(reinterpret_cast<const std::byte*>(&specification) + offsetof(RasterizingPipelineSpecification, m_ColorBlendConstants), sizeof(RasterizingPipelineSpecification) - offsetof(RasterizingPipelineSpecification, m_ColorBlendConstants), 0),
		};

		return static_cast<uint64_t>(XXH64(hashes, sizeof(hashes), 0));
	}

	uint64_t GenerateHash(const VertexDescriptor& descriptor)
	{
		return static_cast<uint64_t>(XXH64(descriptor.data(), sizeof(DataType) * descriptor.size(), 0));
	}
}