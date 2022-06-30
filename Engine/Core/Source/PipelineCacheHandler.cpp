// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/PipelineCacheHandler.hpp"
#include "Core/Errors/AssetError.hpp"

#include <fstream>

namespace Flint
{
	namespace Defaults
	{
		void FilePipelineCacheHandler::store(const std::vector<std::byte>& bytes)
		{
			// Return if we don't have anything to save.
			if (bytes.empty())
				return;

			// Write to file.
			std::fstream cacheFile(m_CacheFile, std::ios::out | std::ios::binary);

			// Write the data if we were able to open the file.
			if (cacheFile.is_open())
				cacheFile.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());

			cacheFile.flush();
			cacheFile.close();
		}

		std::vector<std::byte> FilePipelineCacheHandler::load()
		{
			// Load data from file.
			std::fstream cacheFile(m_CacheFile, std::ios::in | std::ios::ate | std::ios::binary);

			uint64_t size = 0;
			std::vector<std::byte> buffer;

			// If file does not exist, return without an issue.
			if (cacheFile.is_open())
			{
				size = cacheFile.tellg();
				cacheFile.seekg(0);

				buffer.resize(size);
				cacheFile.read(reinterpret_cast<char*>(buffer.data()), size);
			}

			cacheFile.close();
			return buffer;
		}
	}
}