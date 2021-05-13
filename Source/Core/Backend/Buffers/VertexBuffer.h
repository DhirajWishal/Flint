// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DrawResourceBuffer.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Vertex Buffer object.
		 * These buffers submit vertex information to the command buffers.
		 */
		class VertexBuffer : public DrawResourceBuffer
		{
		public:
			VertexBuffer(Device* pDevice, UI64 size) : DrawResourceBuffer(pDevice, size, BufferUsage::VERTEX) {}
		};
	}
}