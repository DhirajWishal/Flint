// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Buffer.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Shader Resource Buffer object.
		 * These buffers submit information directly to shaders. For example: The MVP matrix.
		 */
		class ShaderResourceBuffer : public Buffer
		{
		public:
			ShaderResourceBuffer(Device* pDevice, UI64 size, BufferUsage usage) : Buffer(pDevice, size, usage, MemoryProfile::TRANSFER_FRIENDLY) {}
		};
	}
}