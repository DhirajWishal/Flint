// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.GraphicsCore.CommandSynchronizationPrimitive;

import Flint.GraphicsCore.SynchronizationPrimitive;
import <memory>;

export namespace Flint
{
	/**
	 * Flint command synchronization primitive.
	 * This object is used to synchronize commands.
	 */
	class CommandSynchronizationPrimitive : public SynchronizationPrimitive
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		CommandSynchronizationPrimitive(const std::shared_ptr<Device>& pDevice) : SynchronizationPrimitive(pDevice) {}
	};
}