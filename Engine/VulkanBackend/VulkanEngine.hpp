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
		class VulkanEngine final : public Engine
		{
			/**
			 * Vulkan queue structure.
			 * This contains all the information about a single queue.
			 */
			struct VulkanQueue
			{
				VkQueue m_Queue = VK_NULL_HANDLE;
				uint32_t m_Family = 0;
			};

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
			 * Wait till all the commands have finished execution.
			 */
			void waitIdle() override;

			/**
			 * Create a new window.
			 * @ref Window.hpp
			 *
			 * @param title The window title.
			 * @param width The width of the window. Default is -1.
			 * @param height The height of the window. Default is -1.
			 */
			[[nodiscard]] std::unique_ptr<Window> createWindow(std::string&& title, uint32_t width = -1, uint32_t height = -1) override;

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

			/**
			 * Get the graphics queue from the engine.
			 *
			 * @return The graphics queue.
			 */
			[[nodiscard]] VulkanQueue getGraphicsQueue() const { return m_GraphicsQueue; }

			/**
			 * Get the compute queue from the engine.
			 *
			 * @return The compute queue.
			 */
			[[nodiscard]] VulkanQueue getComputeQueue() const { return m_ComputeQueue; }

			/**
			 * Get the transfer queue from the engine.
			 *
			 * @return The transfer queue.
			 */
			[[nodiscard]] VulkanQueue getTransferQueue() const { return m_TransferQueue; }

			/**
			 * Get the VMA allocator.
			 *
			 * @return The allocator.
			 */
			[[nodiscard]] VmaAllocator getAllocator() const { return m_Allocator; }

		private:
			/**
			 * Select the best physical device for the engine.
			 */
			void selectPhysicalDevice();

			/**
			 * Create a new logical device.
			 */
			void createLogicalDevice();

			/**
			 * Destroy the created logical device.
			 */
			void destroyLogicalDevice();

			/**
			 * Create the VMA allocator.
			 */
			void createVMAAllocator();

			/**
			 * Destroy the VMA allocator.
			 */
			void destroyVMAAllocator();

		private:
			VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};

			VolkDeviceTable m_DeviceTable = {};

			std::vector<const char*> m_DeviceExtensions;

			VulkanQueue m_GraphicsQueue;
			VulkanQueue m_ComputeQueue;
			VulkanQueue m_TransferQueue;

			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;

			VmaAllocator m_Allocator = nullptr;
		};
	}
}