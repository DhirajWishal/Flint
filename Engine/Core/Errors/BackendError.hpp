// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdexcept>

namespace Flint
{
	/**
	 * Asset error class.
	 * This class is thrown if there was any error in the backend.
	 */
	class BackendError final : public std::runtime_error
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param message The message to be thrown.
		 */
		explicit BackendError(const std::string& message) : std::runtime_error(message.c_str()) {}

		/**
		 * Explicit constructor.
		 *
		 * @param message The message to be thrown.
		 */
		explicit BackendError(const char* message) : std::runtime_error(message) {}
	};
}