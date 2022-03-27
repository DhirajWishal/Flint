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
	template<class DeviceT>
	class DeviceSynchronizationPrimitive : public SynchronizationPrimitive<DeviceT>
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		DeviceSynchronizationPrimitive(DeviceT* pDevice) : SynchronizationPrimitive(pDevice) {}
	};
}