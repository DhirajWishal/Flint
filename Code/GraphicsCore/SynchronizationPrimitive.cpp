// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.GraphicsCore.SynchronizationPrimitive;

import Flint.GraphicsCore.DeviceBoundObject;

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
		SynchronizationPrimitive(const std::shared_ptr<Device>& pDevice) : DeviceBoundObject(pDevice) {}
	};
}