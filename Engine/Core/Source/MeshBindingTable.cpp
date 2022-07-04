// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/MeshBindingTable.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

namespace Flint
{

	void MeshBindingTable::bind(uint32_t binding, const std::shared_ptr<Buffer>& pBuffer)
	{
		m_pBuffers[binding] = pBuffer;
	}

	void MeshBindingTable::bind(uint32_t binding, const std::shared_ptr<TextureView>& pView, const std::shared_ptr<TextureSampler>& pSampler, ImageUsage currentUsage)
	{
		// Validate the usage.
		if (currentUsage == (ImageUsage::Graphics | ImageUsage::Storage))
			throw InvalidArgumentError("The current image usage should not be both graphics and storage at the same time!");

		m_Images[binding] = ImageBinding{ pView, pSampler, currentUsage };
	}

	uint64_t MeshBindingTable::generateHash() const
	{
		std::vector<uint64_t> hashes;
		hashes.reserve((m_pBuffers.size() * 2) + (m_Images.size() * 4));

		for (const auto& [index, pBuffer] : m_pBuffers)
		{
			hashes.emplace_back(index);
			hashes.emplace_back(reinterpret_cast<uint64_t>(pBuffer.get()));
		}

		for (const auto& [index, image] : m_Images)
		{
			hashes.emplace_back(index);
			hashes.emplace_back(reinterpret_cast<uint64_t>(image.m_pTextureView.get()));
			hashes.emplace_back(reinterpret_cast<uint64_t>(image.m_pTextureSampler.get()));
			hashes.emplace_back(EnumToInt(image.m_ImageUsage));
		}

		return static_cast<uint64_t>(XXH64(hashes.data(), sizeof(uint64_t) * hashes.size(), 0));
	}
}