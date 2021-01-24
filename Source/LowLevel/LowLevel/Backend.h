// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace Flint
{
	namespace LowLevel
	{
		/**
		 * Supported backend APIs.
		 */
		enum class BackendAPI {
			VULKAN
		};

		/**
		 * Setup backend API.
		 * This function loads the necessary backend to the memory.
		 *
		 * @param api: The API to be loaded.
		 */
		void SetupBackend(const BackendAPI& api);
	}
}