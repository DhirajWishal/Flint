// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Flint synchronization primitive class.
	 * Synchronization primitives are used to synchronize the client applications. These are mainly used in command buffer submissions.
	 */
	class SynchronizationPrimitive : public DeviceBoundObject
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice: The device pointer.
		 */
		SynchronizationPrimitive(const std::shared_ptr<Device>& pDevice) : DeviceBoundObject(pDevice) {}

		/**
		 * Wait until the device flags the primitive.
		 *
		 * @param timeout: The maximum timeout.
		 */
		virtual void Wait(const UI64 timeout = std::numeric_limits<UI64>::max()) = 0;

		/**
		 * Reset the primitive to its default state.
		 */
		virtual void Reset() = 0;
	};
}