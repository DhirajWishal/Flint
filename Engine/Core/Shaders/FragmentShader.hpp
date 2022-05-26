// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"

namespace Flint
{
	/**
	 * Fragment shader class.
	 */
	class FragmentShader final : public Shader
	{
	public:
		// We are using the same constructor(s) and destructor as the super class.
		using Shader::Shader;
		
		/**
		 * Default destructor.
		 */
		~FragmentShader() = default;
	};
}