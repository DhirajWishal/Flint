// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderResourceBuffer.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Uniform Buffer object.
		 * These buffers submit uniform data to the shaders.
		 */
		class UniformBuffer : public ShaderResourceBuffer
		{
		public:
			UniformBuffer(Device* pDevice, UI64 size) : ShaderResourceBuffer(pDevice, size, BufferUsage::UNIFORM) {}
		};
	}
}