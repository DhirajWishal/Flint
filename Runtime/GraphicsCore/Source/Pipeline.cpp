// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/Pipeline.hpp"
#include "GraphicsCore/ResourcePackager.hpp"

#if defined FLINT_PLATFORM_WINDOWS
constexpr const char* CacheDirectory = "\\Flint\\Cache\\";

#elif defined FLINT_PLATFORM_LINUX
constexpr const char* CacheDirectory = "/Flint/Cache/";

#endif // defined FLINT_PLATFORM_WINDOWS


namespace Flint
{
	void Pipeline::WriteDataToCacheFile(const uint64 size, unsigned char* pData) const
	{
		// Return if the name is empty.
		if (mPipelineName.empty())
			return;

		std::ofstream cacheFile(std::filesystem::current_path().string() + CacheDirectory + mPipelineName + ".fpc", std::ios::out | std::ios::binary);

		if (!cacheFile.is_open())
			throw std::runtime_error("Failed to write cache data!");

		cacheFile.write(reinterpret_cast<char*>(pData), size);
		cacheFile.flush();
		cacheFile.close();
	}

	std::pair<uint64, unsigned char*> Pipeline::ReadDataFromCacheFile() const
	{
		// Return if the name is empty.
		if (mPipelineName.empty())
			return std::pair<uint64, unsigned char*>(0, nullptr);

		std::ifstream cacheFile(std::filesystem::current_path().string() + CacheDirectory + mPipelineName + ".fpc", std::ios::in | std::ios::ate | std::ios::binary);

		// If file does not exist, return without an issue.
		if (!cacheFile.is_open())
			return std::pair<uint64, unsigned char*>(0, nullptr);

		const uint64 size = cacheFile.tellg();
		cacheFile.seekg(0);

		unsigned char* pDataStore = new unsigned char[size];
		cacheFile.read(reinterpret_cast<char*>(pDataStore), size);

		cacheFile.close();
		return std::pair<uint64, unsigned char*>(size, pDataStore);
	}

	std::shared_ptr<ResourcePackage> Pipeline::CreateResourcePackage(const uint64 index)
	{
		if (pResourcePackagers.empty())
			CreateResourcePackagers();

		return pResourcePackagers[index]->CreatePackage();
	}
}