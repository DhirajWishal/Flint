// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DrawResourceBuffer.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Index Buffer object.
		 * These buffers submit index information to the command buffers.
		 */
		class IndexBuffer : public DrawResourceBuffer
		{
		public:
			IndexBuffer(Device* pDevice, UI64 size) : DrawResourceBuffer(pDevice, size, BufferUsage::INDEX) {}
		};
	}
}