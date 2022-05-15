// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Engine.hpp"
#include "VulkanInstance.hpp"

#include <vk_mem_alloc.h>

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan engine class.
		 */
		class VulkanEngine final : public EngineCRTP<VulkanInstance>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param instance The instance used to create the object.
			 */
			explicit VulkanEngine(VulkanInstance& instance);

			/**
			 * Destructor.
			 */
			~VulkanEngine() override;

			/**
			 * Get the physical device properties.
			 *
			 * @return The physical device properties.
			 */
			[[nodiscard]] const VkPhysicalDeviceProperties& getPhysicalDeviceProperties() const { return m_PhysicalDeviceProperties; }

			/**
			 * Get the Vulkan functions from the internal device table.
			 *
			 * @return The device table.
			 */
			[[nodiscard]] const VolkDeviceTable& getDeviceTable() const { return m_DeviceTable; }

			/**
			 * Get the Vulkan physical deice.
			 *
			 * @return The Vulkan physical device.
			 */
			[[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }

			/**
			 * Get the Vulkan logical device.
			 *
			 * @return The Vulkan logical device.
			 */
			[[nodiscard]] VkDevice getLogicalDevice() const { return m_LogicalDevice; }

		private:
			/**
			 * Select the best physical device for the engine.
			 */
			void selectPhysicalDevice();

		private:
			VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};

			VolkDeviceTable m_DeviceTable = {};

			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
		};
	}
}