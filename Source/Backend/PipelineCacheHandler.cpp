// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Backend/PipelineCacheHandler.hpp"
#include "Flint/Core/Errors/AssetError.hpp"

#include <Optick.h>

#include <fstream>

namespace Flint
{
	namespace Backend
	{
		namespace Defaults
		{
			FilePipelineCacheHandler::FilePipelineCacheHandler(std::filesystem::path&& path)
				: m_CacheFilePath(std::move(path))
			{
				// Validate the path.
				if (m_CacheFilePath.has_filename())
					throw AssetError("Invlaid path provided! Make sure that the path leads to a directory, not to a file.");

				// If the directory does not exist, let's make it.
				if (!std::filesystem::exists(m_CacheFilePath))
					std::filesystem::create_directories(m_CacheFilePath);
			}

			void FilePipelineCacheHandler::store(uint64_t identifier, const std::vector<std::byte>& bytes)
			{
				OPTICK_EVENT();

				// Return if we don't have anything to save.
				if (bytes.empty())
					return;

				// Write to file.
				const auto filename = std::to_string(identifier) + ".fpc";
				std::fstream cacheFile(m_CacheFilePath / filename, std::ios::out | std::ios::binary);

				// Write the data if we were able to open the file.
				if (cacheFile.is_open())
					cacheFile.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());

				cacheFile.flush();
				cacheFile.close();
			}

			std::vector<std::byte> FilePipelineCacheHandler::load(uint64_t identifier)
			{
				OPTICK_EVENT();

				// Load data from file.
				const auto filename = std::to_string(identifier) + ".fpc";
				std::fstream cacheFile(m_CacheFilePath / filename, std::ios::in | std::ios::ate | std::ios::binary);

				std::vector<std::byte> buffer;

				// If file does not exist, return without an issue.
				if (cacheFile.is_open())
				{
					const auto size = cacheFile.tellg();
					cacheFile.seekg(0);

					buffer.resize(size);
					cacheFile.read(reinterpret_cast<char*>(buffer.data()), size);
				}

				cacheFile.close();
				return buffer;
			}
		}
	}
}