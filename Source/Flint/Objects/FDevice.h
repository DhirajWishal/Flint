// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FInstance.h"
#include "Core\Backend\Buffer.h"

namespace Flint
{
	class FDisplay;

	class FScreenBoundRenderTarget;
	class FBuffer;

	/**
	 * Flint device object.
	 * This object is responsible for all the asset creations and their lifetimes.
	 */
	class FDevice final : public FObject {
	public:
		FDevice();
		explicit FDevice(const std::shared_ptr<Backend::BackendObject>& pBackendObject) : FObject(pBackendObject) {}
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

	public:
		/**
		 * Create a new screen bound render target object.
		 * 
		 * @param display: The display object it is bound to.
		 * @param bufferCount: The number of frame buffers to use. If set to 0, the default count is set. If set to std::numeric_limits<UI64>::max(), the maximum supported is set.
		 * @return The newly created render target object.
		 */
		FScreenBoundRenderTarget CreateScreenBoundRenderTarget(const FDisplay& display, UI64 bufferCount) const;

		/**
		 * Create a new buffer object.
		 * 
		 * @param size: The size of the buffer.
		 * @param usage: The buffer usage.
		 * @param profile: The buffer's memory profile.
		 */
		FBuffer CreateBuffer(UI64 size, Backend::BufferUsage usage, Backend::MemoryProfile profile) const;

	private:
		bool bShouldDelete = false;
	};
}