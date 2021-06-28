// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TransferFriendlyBuffer.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint stagging buffer object.
		 * Stagging buffers are used to submit data to the device and is used to transfer data between other buffers and images.
		 */
		class StaggingBuffer : public TransferFriendlyBuffer
		{
		public:
			StaggingBuffer(Device& device, UI64 size) : TransferFriendlyBuffer(device, size) {}
		};
	}
}