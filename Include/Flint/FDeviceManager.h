// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDisplayManager.h"

namespace Flint
{
	class FDeviceManager;

	/**
	 * Flint device handle struct.
	 */
	struct FLINT_API FDeviceHandle
	{
		FDeviceHandle() = default;
		FDeviceHandle(FDeviceManager* pManager, UI8 index) : pManager(pManager), mIndex(index) {}

		operator const UI8() const { return mIndex; }

	private:
		FDeviceManager* pManager = nullptr;
		UI8 mIndex = 0;
	};

	/**
	 * Flint device manager object.
	 * This object is responsible for all the device based operations.
	 */
	class FLINT_API FDeviceManager
	{
	public:
		FDeviceManager() = default;

		/**
		 * Create a new device handle.
		 * 
		 * @return The created display handle.
		 */
		virtual FDeviceHandle CreateDevice() = 0;

		/**
		 * Terminate a created device.
		 * 
		 * @param handle: The handle to be terminated.
		 */
		virtual void Terminate(FDeviceHandle& handle) = 0;
	};
}