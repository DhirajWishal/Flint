// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Utility/Hasher.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

namespace Flint
{
	uint64_t GenerateHash(const ResourceBindingTable& table)
	{
		const XXH64_hash_t hashes[] = {
			XXH64(table.m_Images.data(), sizeof(BinaryMap<uint32_t, ImageHandle>::entry_type) * table.m_Images.size(), 0),
			XXH64(table.m_Buffers.data(), sizeof(BinaryMap<uint32_t, BufferHandle>::entry_type) * table.m_Buffers.size(), 0)
		};

		return static_cast<uint64_t>(XXH64(hashes, sizeof(hashes), 0));
	}

	uint64_t GenerateHash(const RasterizingPipelineSpecification& specification)
	{
		using PathCharacterType = std::filesystem::path::string_type::value_type;

		// Generate the hashes for the input binding.
		std::vector<XXH64_hash_t> inputBindingHash;
		inputBindingHash.reserve(specification.m_InputBindings.size());
		for (const auto& binding : specification.m_InputBindings)
		{
			const XXH64_hash_t hashes[] = {
				XXH64(&binding.m_Type, sizeof(InputBindingType), 0),
				XXH64(binding.m_Attributes.data(), sizeof(std::pair<uint32_t, VertexAttributeType>) * binding.m_Attributes.size(), 0)
			};

			inputBindingHash.emplace_back(XXH64(hashes, sizeof(hashes), 0));
		}

		// Generate the hash for the whole structure.
		const XXH64_hash_t hashes[] = {
			XXH64(specification.m_CacheFile.native().data(), sizeof(PathCharacterType) * specification.m_CacheFile.native().size(), 0),
			XXH64(specification.m_VertexShader.getCode().data(), sizeof(uint32_t) * specification.m_VertexShader.getCode().size(), 0),
			XXH64(specification.m_TessellationControlShader.getCode().data(), sizeof(uint32_t) * specification.m_TessellationControlShader.getCode().size(), 0),
			XXH64(specification.m_TessellationEvaluationShader.getCode().data(), sizeof(uint32_t) * specification.m_TessellationEvaluationShader.getCode().size(), 0),
			XXH64(specification.m_GeometryShader.getCode().data(), sizeof(uint32_t) * specification.m_GeometryShader.getCode().size(), 0),
			XXH64(specification.m_FragmentShader.getCode().data(), sizeof(uint32_t) * specification.m_FragmentShader.getCode().size(), 0),
			XXH64(inputBindingHash.data(), sizeof(XXH64_hash_t) * inputBindingHash.size(), 0),
			XXH64(reinterpret_cast<const std::byte*>(&specification) + offsetof(RasterizingPipelineSpecification, m_ColorBlendConstants), sizeof(RasterizingPipelineSpecification) - offsetof(RasterizingPipelineSpecification, m_ColorBlendConstants), 0),
		};

		return static_cast<uint64_t>(XXH64(hashes, sizeof(hashes), 0));
	}
}