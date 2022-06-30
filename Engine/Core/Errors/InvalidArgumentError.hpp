// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdexcept>

namespace Flint
{
	/**
	 * Invalid argument error class.
	 * This class is thrown if there was any errors with arguments.
	 */
	class InvalidArgumentError final : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};
}