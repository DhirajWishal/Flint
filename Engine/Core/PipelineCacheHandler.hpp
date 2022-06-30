// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

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
		 * @param bytes The cache data to store.
		 */
		virtual void store(const std::vector<std::byte>& bytes) = 0;

		/**
		 * Load cache data.
		 *
		 * @return The loaded cache data.
		 */
		[[nodiscard]] virtual std::vector<std::byte> load() = 0;
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
			 * @param path The file path of the cache file.
			 */
			explicit FilePipelineCacheHandler(std::filesystem::path&& path) : m_CacheFile(std::move(path)) {}

			/**
			 * Store the cache data.
			 *
			 * @param bytes The cache data to store.
			 */
			void store(const std::vector<std::byte>& bytes) override;

			/**
			 * Load cache data.
			 *
			 * @return The loaded cache data.
			 */
			[[nodiscard]] std::vector<std::byte> load() override;

		private:
			const std::filesystem::path m_CacheFile;
		};
	}
}