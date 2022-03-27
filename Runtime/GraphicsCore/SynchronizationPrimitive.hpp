// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Flint synchronization primitive class.
	 * Synchronization primitives are used to synchronize the client applications. These are mainly used in command buffer submissions.
	 */
	template<class DeviceT>
	class SynchronizationPrimitive : public DeviceBoundObject<DeviceT>
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		SynchronizationPrimitive(DeviceT* pDevice) : DeviceBoundObject(pDevice) {}
	};
}