// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

namespace Flint
{
	/**
	 * Flint device object.
	 * This object is the basis for all the resources. Almost all the resources are bound to a device.
	 */
	class FLINT_API Device : public FObject
	{
	public:
		Device(Instance& instance) : mInstance(instance) {}

	protected:
		Instance& mInstance;
	};
}