// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderResourceBuffer.hpp"

namespace Flint
{
	/**
	 * Flint uniform buffer object.
	 * Uniform buffers are used to store and submit uniform data to the shaders.
	 */
	class FLINT_API UniformBuffer : public ShaderResourceBuffer
	{
	public:
		UniformBuffer(Device& device, UI64 size) : ShaderResourceBuffer(device, size) {}
	};
}