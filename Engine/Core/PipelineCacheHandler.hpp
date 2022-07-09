// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <filesystem>

namespace Flint
{
	/**
	 * Pipeline cache handler class.
	 * This class is used to handle pipeline cache storing and loading.
	 */
	class PipelineCacheHandler
	{
	public:
		/**
		 * Default constructor.
		 */
		PipelineCacheHandler() = default;

		/**
		 * Default virtual destructor.
		 */
		virtual ~PipelineCacheHandler() = default;

		/**
		 * Store the cache data.
		 *
		 * @param identifier The pipeline identifier.
		 * @param bytes The cache data to store.
		 */
		virtual void store(uint64_t identifier, const std::vector<std::byte>& bytes) = 0;

		/**
		 * Load cache data.
		 *
		 * @param identifier The pipeline identifier.
		 * @return The loaded cache data.
		 */
		[[nodiscard]] virtual std::vector<std::byte> load(uint64_t identifier) = 0;
	};

	namespace Defaults
	{
		/**
		 * File pipeline cache handler class.
		 * This is a default pipeline cache handler provided by Flint where the pipeline cache is stored in a binary file with the *.fpc extension (Flint Pipeline Cache).
		 */
		class FilePipelineCacheHandler final : public PipelineCacheHandler
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param path The file path to store the cache. Make sure that it is a directory, not a file.
			 */
			explicit FilePipelineCacheHandler(std::filesystem::path&& path);

			/**
			 * Store the cache data.
			 *
			 * @param identifier The pipeline identifier.
			 * @param bytes The cache data to store.
			 */
			void store(uint64_t identifier, const std::vector<std::byte>& bytes) override;

			/**
			 * Load cache data.
			 *
			 * @param identifier The pipeline identifier.
			 * @return The loaded cache data.
			 */
			[[nodiscard]] std::vector<std::byte> load(uint64_t identifier) override;

		private:
			const std::filesystem::path m_CacheFilePath;
		};
	}
}