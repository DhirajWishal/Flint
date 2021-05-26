// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderResourceBuffer.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Storage Buffer object.
		 * These buffers act similarly to the Uniform Buffer but its main purpose is to store prepossessed information.
		 */
		class StorageBuffer : public ShaderResourceBuffer
		{
		public:
			StorageBuffer(Device* pDevice, UI64 size) : ShaderResourceBuffer(pDevice, size, BufferUsage::STORAGE) {}
		};
	}
}