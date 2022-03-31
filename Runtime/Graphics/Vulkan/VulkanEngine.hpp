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

		/**
		 * Get the instance pointer.
		 * 
		 * @return The Vulkan instance pointer.
		 */
		VulkanBackend::VulkanInstance* GetInstance() { return &m_Instance; }

		/**
		 * Get the instance pointer.
		 *
		 * @return The Vulkan instance pointer.
		 */
		const VulkanBackend::VulkanInstance* GetInstance() const { return &m_Instance; }

		/**
		 * Create a new graphics reactor.
		 * 
		 * @param The required device flags. Default is external, graphics and compute compatible.
		 * @return The graphics reactor.
		 */
		std::unique_ptr<GraphicsReactor> CreateReactor(DeviceFlags flags = DeviceFlags::External | DeviceFlags::GraphicsCompatible | DeviceFlags::ComputeCompatible) override;

	private:
		VulkanBackend::VulkanInstance m_Instance;
	};
}