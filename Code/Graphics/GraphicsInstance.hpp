// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Instance.hpp"
#include "GraphicsDevice.hpp"

namespace Flint
{
	/**
	 * Flint graphics engine.
	 * This object house a single graphics engine instance.
	 */
	class GraphicsInstance final
	{
		/**
		 * Constructor.
		 * This will create a new backend instance.
		 *
		 * @param enableValidation Whether or not to enable backend validation.
		 */
		GraphicsInstance();

		/**
		 * Virtual destructor.
		 * This will terminate the instance automatically.
		 */
		virtual ~GraphicsInstance();

		/**
		 * Create a new graphics device.
		 *
		 * @return The graphics device pointer.
		 */
		virtual std::shared_ptr<GraphicsDevice> CreateGraphicsDevice() = 0;

	public:
		/**
		 * Get the primitive instance pointer.
		 *
		 * @return The instance shared pointer.
		 */
		std::shared_ptr<Instance> GetPrimitive() const { return pInstance; }

	protected:
		std::shared_ptr<Instance> pInstance = nullptr;
	};
}