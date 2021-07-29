// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ResourceMap.hpp"

namespace Flint
{
	ResourceMap::ResourceMap(const std::vector<std::string>& bufferResourceNames)
	{
		for (const std::string name : bufferResourceNames)
			pBufferMap[name] = nullptr;
	}

	bool ResourceMap::SetResource(const std::string& name, const std::shared_ptr<Buffer>& pBuffer)
	{
		// If the resource name does not exist, return false.
		if (pBufferMap.find(name) == pBufferMap.end())
			return false;

		pBufferMap[name] = pBuffer;
		return true;
	}
	
	bool ResourceMap::operator==(const ResourceMap& other) const
	{
		return pBufferMap == other.pBufferMap;
	}
}