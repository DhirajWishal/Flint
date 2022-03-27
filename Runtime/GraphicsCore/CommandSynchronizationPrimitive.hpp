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
	class CommandSynchronizationPrimitive : public SynchronizationPrimitive
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		CommandSynchronizationPrimitive(Device* pDevice) : SynchronizationPrimitive(pDevice) {}
	};
}