// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ResourceMap.hpp"

namespace Flint
{
	ResourceMap::ResourceMap(const std::vector<ShaderResourceKey>& bufferResourceKeys, const std::vector<ShaderResourceKey>& imageResourceKeys)
	{
		for (const ShaderResourceKey key : bufferResourceKeys)
			mBufferMap[key] = {};

		for (const ShaderResourceKey key : imageResourceKeys)
			mImageMap[key] = {};
	}

	void ResourceMap::SetResource(const ShaderResourceKey& key, const std::shared_ptr<Buffer>& pBuffer, const UI64 offset)
	{
		// If the resource key does not exist, return false.
		if (mBufferMap.find(key) == mBufferMap.end())
			throw std::invalid_argument("Buffer with the given binding key {\"" + std::to_string(key.mSetIndex) + ", " + std::to_string(key.mBindingIndex) + "\"} is not present within the resource map!");

		mBufferMap[key] = BufferBinding(pBuffer, offset);
	}

	void ResourceMap::SetResource(const ShaderResourceKey& key, const std::shared_ptr<ImageSampler>& pSampler, const std::shared_ptr<Image>& pImage)
	{
		// If the resource key does not exist, return false.
		if (mImageMap.find(key) == mImageMap.end())
			throw std::invalid_argument("Image with the given binding key {\"" + std::to_string(key.mSetIndex) + ", " + std::to_string(key.mBindingIndex) + "\"} is not present within the resource map!");

		mImageMap[key] = ImageBinding(pSampler, pImage);
	}

	bool ResourceMap::operator==(const ResourceMap& other) const
	{
		return mBufferMap == other.mBufferMap && mImageMap == other.mImageMap;
	}
}