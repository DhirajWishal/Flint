// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphics/GraphicsEngine.hpp"
#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	/**
	 * Vulkan engine class.
	 * This class contains the Vulkan engine instance and is responsible for creating the required Vulkan device.
	 */
	class VulkanEngine final : public GraphicsEngine
	{
	public:
		/**
		 * Constructor.
		 * 
		 * @param enableValidation Enable backend validation or not. If enabled (as default), it may result in a performance drop as it requires more resources.
		 */
		explicit VulkanEngine(bool enableValidation = true);

	private:
		VulkanInstance m_Instance;
	};
}