// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderResourceBuffer.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint storage buffer object.
		 * Storage buffers are used to store data from the shader output.
		 */
		class StorageBuffer : public ShaderResourceBuffer
		{
		public:
			StorageBuffer(const std::shared_ptr<Device>& pDevice, UI64 size) : ShaderResourceBuffer(pDevice, size) {}
		};
	}
}
