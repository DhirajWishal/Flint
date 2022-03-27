// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SynchronizationPrimitive.hpp"

namespace Flint
{
	/**
	 * Flint host synchronization primitive.
	 * This primitive is used to synchronize the host application (the application that is using the API).
	 */
	class HostSynchronizationPrimitive : public SynchronizationPrimitive
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		HostSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice) : SynchronizationPrimitive(pDevice) {}

		/**
		 * Wait until the device flags the primitive.
		 *
		 * @param timeout The maximum timeout.
		 */
		virtual void Wait(const uint64_t timeout = std::numeric_limits<uint64_t>::max()) = 0;

		/**
		 * Reset the primitive to its default state.
		 */
		virtual void Reset() = 0;
	};
}