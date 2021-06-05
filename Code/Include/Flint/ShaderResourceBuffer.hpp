// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TransferFriendlyBuffer.hpp"

namespace Flint
{
	/**
	 * Flint shader resource buffer object.
	 * These buffers are used to submit data to the shaders at draw time.
	 */
	class FLINT_API ShaderResourceBuffer : public TransferFriendlyBuffer
	{
	public:
		ShaderResourceBuffer(Device& device, UI64 size) : TransferFriendlyBuffer(device, size) {}
	};
}