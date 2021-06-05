// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Flint buffer object.
	 * Buffers are used to store data in the device.
	 */
	class FLINT_API Buffer : public DeviceBoundObject
	{
	public:
		Buffer(Device& device, const UI64 size) : DeviceBoundObject(device), mSize(size) {}

	public:
		/**
		 * Get the buffer size.
		 *
		 * @return The size in bytes.
		 */
		UI64 GetSize() const { return mSize; }

	protected:
		UI64 mSize = 0;
	};
}