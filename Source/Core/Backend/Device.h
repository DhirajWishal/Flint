// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

namespace Flint
{
	class FDisplay;
	class FBuffer;

	/**
	 * Flint device object.
	 * Device objects are the basis of all the assets. This resembles a single (or multiple in the future) GPU(s).
	 */
	class FDevice : public BackendObject {
	public:
		FDevice() {}
		virtual ~FDevice() {}

		/**
		 * Initialize the device.
		 *
		 * @param pIstance: The instance object pointer which the device is made to.
		 */
		virtual void Initialize(FInstance* pInstance) = 0;

		/**
		 * Check if the device and display is compatible.
		 *
		 * @return Boolean value stating the validity.
		 */
		virtual bool CheckDisplayCompatibility(FDisplay* pDisplay) = 0;

		/**
		 * Terminate the device.
		 */
		virtual void Terminate() = 0;

		/**
		 * Get the instance pointer.
		 *
		 * @return The instance object pointer.
		 */
		FInstance* GetInstance() const { return pInstance; }

	public:
		/**
		 * Create a new buffer object.
		 * 
		 * @param size: The size of the buffer.
		 * @param usage: The buffer usage.
		 * @param memoryProfile: The buffer's memory profile.
		 * @return The created buffer pointer.
		 */
		virtual FBuffer* CreateBuffer(UI64 size, BufferUsage usage, MemoryProfile memoryProfile) = 0;

	protected:
		FInstance* pInstance = nullptr;
	};
}