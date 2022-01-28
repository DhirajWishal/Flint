// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.GraphicsCore.DeviceSynchronizationPrimitive;

import Flint.GraphicsCore.SynchronizationPrimitive;
import <memory>;

export namespace Flint
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
		DeviceSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice) : SynchronizationPrimitive(pDevice) {}
	};
}