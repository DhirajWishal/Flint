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
	class SynchronizationPrimitive : public DeviceBoundObject
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		SynchronizationPrimitive(Device* pDevice) : DeviceBoundObject(pDevice) {}
	};
}