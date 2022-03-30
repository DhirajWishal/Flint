// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Instance.hpp"
#include "GraphicsDevice.hpp"

#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	/**
	 * Flint graphics engine.
	 * This object house a single graphics engine instance.
	 */
	class GraphicsEngine final
	{
	public:
		/**
		 * Constructor.
		 * This will create a new backend instance.
		 *
		 * @param enableValidation Whether or not to enable backend validation.
		 */
		explicit GraphicsEngine(bool enableValidation);

		/**
		 * Destructor.
		 * This will terminate the instance automatically.
		 */
		~GraphicsEngine();

		/**
		 * Create a new graphics device.
		 *
		 * @return The graphics device pointer.
		 */
		std::shared_ptr<GraphicsDevice> CreateGraphicsDevice();
	};
}