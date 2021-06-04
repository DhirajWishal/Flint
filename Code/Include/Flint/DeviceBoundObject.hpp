// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.hpp"
#include <string>

namespace Flint
{
	class Device;

	/**
	 * Flint device bound object.
	 * This object is the base class for all the resources in Flint which are based on a device.
	 */
	class FLINT_API DeviceBoundObject : public FObject
	{
		friend Device;

	public:
		DeviceBoundObject(Device& device) noexcept : mDevice(device) {}

	protected:
		/**
		 * Terminate the device bound object.
		 */
		virtual void Terminate() = 0;

	protected:
		Device& mDevice;
	};
}