// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Buffer.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Draw Resource Buffer object.
		 * These buffers submit draw data using command buffers to be drawn by a pipeline.
		 *
		 * In order to submit data to these buffers, another buffer must be used. Presumably a stagging buffer.
		 */
		class DrawResourceBuffer : public Buffer
		{
		public:
			DrawResourceBuffer(Device* pDevice, UI64 size, BufferUsage usage) : Buffer(pDevice, size, usage, MemoryProfile::DRAW_RESOURCE) {}
		};
	}
}