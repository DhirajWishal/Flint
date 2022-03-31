// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphics/GraphicsReactor.hpp"

namespace Flint
{
	/**
	 * Vulkan reactor class.
	 * This is the API specific reactor for Vulkan.
	 */
	class VulkanReactor final : public GraphicsReactor
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pEngine The engine to which this object is bound to.
		 * @param flags The required device flags. Default is external, graphics and compute compatible.
		 */
		explicit VulkanReactor(GraphicsEngine* pEngine, DeviceFlags flags);

		/**
		 * Worker function.
		 * 
		 * @param pEngine The engine to which this reactor is bound to.
		 */
		void Worker(GraphicsEngine* pEngine, DeviceFlags flags) override;
	};
}