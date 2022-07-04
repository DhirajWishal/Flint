// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdexcept>

namespace Flint
{
	/**
	 * Invalid backend error class.
	 * This class is thrown if the provided backend was invalid.
	 */
	class InvalidBackend final : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};
}