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
}