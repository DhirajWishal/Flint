// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

namespace Flint
{
	/**
	 * Backend API enum.
	 */
	enum class BackendAPI : uint8_t
	{
		Vulkan
	};

	/**
	 * Instance class.
	 * Every application requires just one instance of this class. It contains all the essential information, like validation and other information required by graphics engines.
	 */
	class Instance
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param api The backend API type.
		 */
		explicit Instance(BackendAPI api) : m_BackendAPI(api) {}

		/**
		 * Get the backend API.
		 *
		 * @return The backend API type.
		 */
		[[nodiscard]] BackendAPI getBackendAPI() const { return m_BackendAPI; }

	private:
		const BackendAPI m_BackendAPI;
	};
}