// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

namespace Flint
{
	class Display;

	/**
	 * Flint device object.
	 * This object is the basis for all the resources. Almost all the resources are bound to a device.
	 */
	class FLINT_API Device : public FObject
	{
	public:
		Device(Instance& instance, DeviceFlags flags) noexcept : mInstance(instance), mFlags(flags) {}

		/**
		 * Check if the display is compatible with the device.
		 * 
		 * @param display: The display to check.
		 * @return Boolean value stating if compatible or not.
		 */
		virtual bool IsDisplayCompatible(const Display& display) = 0;

	protected:
		Instance& mInstance;
		DeviceFlags mFlags = DeviceFlags::EXTERNAL | DeviceFlags::GRAPHICS_COMPATIBLE;
	};
}