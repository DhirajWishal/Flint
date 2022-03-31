// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Instance.hpp"
#include "GraphicsReactor.hpp"

#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	/**
	 * Flint graphics engine.
	 * This object house a single graphics engine instance.
	 */
	class GraphicsEngine
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
		virtual ~GraphicsEngine();

		/**
		 * Create a new graphics reactor.
		 *
		 * @param flags The required device flags. Default is external, graphics and compute compatible.
		 * @return The graphics reactor.
		 */
		virtual std::unique_ptr<GraphicsReactor> CreateReactor(DeviceFlags flags = DeviceFlags::External | DeviceFlags::GraphicsCompatible | DeviceFlags::ComputeCompatible) = 0;
	};
}