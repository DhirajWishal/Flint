// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SynchronizationPrimitive.hpp"

namespace Flint
{
	/**
	 * Flint device synchronization primitive.
	 * This object is used to synchronize the device operations.
	 */
	class DeviceSynchronizationPrimitive : public SynchronizationPrimitive
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		DeviceSynchronizationPrimitive(Device* pDevice) : SynchronizationPrimitive(pDevice) {}
	};
}