// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderResourceBuffer.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint uniform buffer object.
		 * Uniform buffers are used to store and submit uniform data to the shaders.
		 */
		class UniformBuffer : public ShaderResourceBuffer
		{
		public:
			UniformBuffer(const std::shared_ptr<Device>& pDevice, UI64 size) : ShaderResourceBuffer(pDevice, size) {}
		};
	}
}