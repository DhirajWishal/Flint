// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Pipeline.hpp"

namespace Flint
{
	namespace Helpers
	{
		ResourceDescriptor::BufferAttachment CreateBufferResourceAttachment(const std::string& resourceName, const std::shared_ptr<Buffer>& pBuffer)
		{
			return ResourceDescriptor::BufferAttachment(resourceName, pBuffer);
		}
	}

	void Pipeline::WriteDataToCacheFile(UI64 size, unsigned char* pData) const
	{
		// Return if the name is empty.
		if (mPipelineName.empty())
			return;

		std::ofstream cacheFile(std::filesystem::current_path().string() + FLINT_CACHE_DIRECTORY "\\" + mPipelineName + ".fpc", std::ios::binary);

		if (!cacheFile.is_open())
			FLINT_THROW_RUNTIME_ERROR("Failed to write cache data!");

		cacheFile.write(reinterpret_cast<char*>(pData), size);
		cacheFile.flush();
		cacheFile.close();
	}

	std::pair<UI64, unsigned char*> Pipeline::ReadDataFromCacheFile() const
	{
		// Return if the name is empty.
		if (mPipelineName.empty())
			return std::pair<UI64, unsigned char*>(0, nullptr);

		std::ifstream cacheFile(std::filesystem::current_path().string() + FLINT_CACHE_DIRECTORY "\\" + mPipelineName + ".fpc", std::ios::ate | std::ios::binary);

		// If file does not exist, return without an issue.
		if (!cacheFile.is_open())
			return std::pair<UI64, unsigned char*>(0, nullptr);

		UI64 size = cacheFile.tellg();
		cacheFile.seekg(0);

		unsigned char* pDataStore = new unsigned char[size];
		cacheFile.read(reinterpret_cast<char*>(pDataStore), size);

		cacheFile.close();
		return std::pair<UI64, unsigned char*>(size, pDataStore);
	}
}