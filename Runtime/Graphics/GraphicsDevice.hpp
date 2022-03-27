// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"

namespace Flint
{
	class GraphicsEngine;

	/**
	 * Flint graphics device object.
	 */
	class GraphicsDevice
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pEngine The engine pointer.
		 */
		GraphicsDevice(GraphicsEngine* pEngine);

		/**
		 * Virtual destructor.
		 * This will automatically terminate the device.
		 */
		virtual ~GraphicsDevice();

	public:
		/**
		 * Get the primitive device pointer.
		 *
		 * @reutrn The device shared pointer.
		 */
		Device* GetPrimitive() const { return pDevice.get(); }

	protected:
		std::unique_ptr<Device> pDevice = nullptr;
	};
}