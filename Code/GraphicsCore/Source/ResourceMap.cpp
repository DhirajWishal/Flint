// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ResourceMap.hpp"

namespace Flint
{
	ResourceMap::ResourceMap(const std::vector<std::string>& bufferResourceNames, const std::vector<std::string>& imageResourceNames)
	{
		for (const std::string name : bufferResourceNames)
			pBufferMap[name] = nullptr;

		for (const std::string name : imageResourceNames)
			pImageMap[name] = {};
	}

	void ResourceMap::SetResource(const std::string& name, const std::shared_ptr<Buffer>& pBuffer)
	{
		// If the resource name does not exist, return false.
		if (pBufferMap.find(name) == pBufferMap.end())
			FLINT_THROW_INVALID_ARGUMENT("Buffer with the given name \"" + name + "\" is not present within the resource map!");

		pBufferMap[name] = pBuffer;
	}

	void ResourceMap::SetResource(const std::string& name, const std::shared_ptr<ImageSampler>& pSampler, const std::shared_ptr<Image>& pImage)
	{
		// If the resource name does not exist, return false.
		if (pImageMap.find(name) == pImageMap.end())
			FLINT_THROW_INVALID_ARGUMENT("Image with the given name \"" + name + "\" is not present within the resource map!");

		pImageMap[name] = std::pair(pSampler, pImage);
	}

	bool ResourceMap::operator==(const ResourceMap& other) const
	{
		return pBufferMap == other.pBufferMap && pImageMap == other.pImageMap;
	}
}