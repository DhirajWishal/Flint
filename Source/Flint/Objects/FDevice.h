// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FInstance.h"

namespace Flint
{
	constexpr const UI64 GetDeviceSize();
	constexpr const UI64 GetDeviceAlignment();

	/**
	 * Flint device object.
	 * This object is responsible for all the asset creations and their lifetimes.
	 */
	class FDevice final : FObject<GetDeviceSize(), GetDeviceAlignment()> {
	public:
		FDevice();
		~FDevice();

		/**
		 * Initialize the device.
		 * 
		 * @param instance: The instance object used to create the device.
		 */
		void Initialize(const FInstance& instance);

		/**
		 * Terminate the device.
		 */
		void Terminate();
	};
}