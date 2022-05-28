// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdexcept>

namespace Flint
{
	/**
	 * Shader builder error class.
	 * This error is thrown if an error has been occurred in the shader builder.
	 */
	class ShaderBuilderError final : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};
}