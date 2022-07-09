// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdexcept>

namespace Flint
{
	/**
	 * Asset error class.
	 * This class is thrown if there was any error with assets.
	 */
	class AssetError final : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};
}