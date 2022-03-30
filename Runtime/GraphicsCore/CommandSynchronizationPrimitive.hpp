// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SynchronizationPrimitive.hpp"

namespace Flint
{
	/**
	 * Flint command synchronization primitive.
	 * This object is used to synchronize commands.
	 */
	template<class DeviceT>
	class CommandSynchronizationPrimitive : public SynchronizationPrimitive<DeviceT>
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		explicit CommandSynchronizationPrimitive(DeviceT* pDevice) : SynchronizationPrimitive(pDevice) {}
	};
}