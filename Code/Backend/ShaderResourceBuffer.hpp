// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TransferFriendlyBuffer.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint shader resource buffer object.
		 * These buffers are used to submit data to the shaders at draw time.
		 */
		class ShaderResourceBuffer : public TransferFriendlyBuffer
		{
		public:
			ShaderResourceBuffer(const std::shared_ptr<Device>& pDevice, UI64 size) : TransferFriendlyBuffer(pDevice, size) {}
		};
	}
}