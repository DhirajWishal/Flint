// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderResourceBuffer.hpp"

namespace Flint
{
	/**
	 * Flint storage buffer object.
	 * Storage buffers are used to store data from the shader output.
	 */
	class FLINT_API StorageBuffer : public ShaderResourceBuffer
	{
	public:
		StorageBuffer(Device& device, UI64 size) : ShaderResourceBuffer(device, size) {}
	};
}
