// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

namespace Flint
{
	/**
	 * Flint device object.
	 * This object is the basis for all the resources. Almost all the resources are bound to a device.
	 */
	class FLINT_API Device : public FObject
	{
	public:
		Device(Instance& instance, DeviceFlags flags) noexcept : mInstance(instance), mFlags(flags) {}

	protected:
		Instance& mInstance;
		DeviceFlags mFlags = DeviceFlags::EXTERNAL | DeviceFlags::GRAPHICS_COMPATIBLE;
	};
}