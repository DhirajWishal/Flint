// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"

namespace Flint
{
	/**
	 * Flint device bound object.
	 * This object is the base class for all the resources in Flint which are based on a device.
	 */
	class FLINT_API DeviceBoundObject : public FObject
	{
	public:
		DeviceBoundObject(Device& device) : mDevice(device) {}

	protected:
		Device& mDevice;
	};
}