// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Engine.hpp"
#include "Core/Types.hpp"
#include "VulkanInstance.hpp"

#include <vk_mem_alloc.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBuffers;
		class VulkanGeometryStore;

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
			 * Get the best depth pixel format.
			 *
			 * @return The pixel format.
			 */
			[[nodiscard]] PixelFormat getBestDepthFormat() const override;

			/**
			 * Get the max supported multisample count.
			 *
			 * @return The multisample count.
			 */
			[[nodiscard]] Multisample getMaximumMultisample() const override;

			/**
			 * Create a new window.
			 * @ref Window.hpp
			 *
			 * @param title The window title.
			 * @param width The width of the window. Default is -1.
			 * @param height The height of the window. Default is -1.
			 * @return The created window pointer.
			 */
			[[nodiscard]] std::unique_ptr<Window> createWindow(std::string&& title, uint32_t width = -1, uint32_t height = -1) override;

			/**
			 * Create a new rasterizer.
			 * @ref Rasterizer.hpp
			 *
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
			 * @param attachmentDescriptions The attachment descriptions.
			 * @param multisample The multisample count. Default is One.
			 * @param exclusiveBuffering Whether or not to use one buffer/ attachment per frame. Default is false.
			 * @return The created rasterizer pointer.
			 */
			[[nodiscard]] std::unique_ptr<Rasterizer> createRasterizer(uint32_t width, uint32_t height, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One, bool exclusiveBuffering = false) override;

			/**
			 * Create a new geometry store.
			 *
			 * @return The created geometry store.
			 */
			[[nodiscard]] std::unique_ptr<GeometryStore> createGeometryStore() override;

			/**
			 * Get the default geometry store.
			 *
			 * @return The geometry store reference.
			 */
			[[nodiscard]] GeometryStore& getDefaultGeometryStore() override;

			/**
			 * Get the default geometry store.
			 *
			 * @return The geometry store reference.
			 */
			[[nodiscard]] const GeometryStore& getDefaultGeometryStore() const override;

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

			/**
			 * Get the utility command buffer.
			 *
			 * @return The command buffer reference.
			 */
			[[nodiscard]] VulkanCommandBuffers& getUtilityCommandBuffer() { return *m_pUtilityCommandBuffer; }

			/**
			 * Get the utility command buffer.
			 *
			 * @return The const command buffer reference.
			 */
			[[nodiscard]] const VulkanCommandBuffers& getUtilityCommandBuffer() const { return *m_pUtilityCommandBuffer; }

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

			VulkanCommandBuffers* m_pUtilityCommandBuffer = nullptr;
			VulkanGeometryStore* m_pDefaultGeometryStore = nullptr;

			VulkanQueue m_GraphicsQueue;
			VulkanQueue m_ComputeQueue;
			VulkanQueue m_TransferQueue;

			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;

			VmaAllocator m_Allocator = nullptr;
		};

		namespace Utility
		{
			/**
			 * Get the Vulkan sample count flag bits from multisample.
			 *
			 * @param multisample The multisample count.
			 * @return The Vulkan sample count.
			 */
			[[nodiscard]] VkSampleCountFlagBits GetSampleCountFlagBits(Multisample multisample);

			/**
			 * Get the Vulkan image format from pixel format.
			 *
			 * @param format The pixel format.
			 * @return The Vulkan format.
			 */
			[[nodiscard]] VkFormat GetImageFormat(PixelFormat format);

			/**
			 * Get the pixel format from the Vulkan image format.
			 *
			 * @param format The Vulkan format.
			 * @return The pixel format.
			 */
			[[nodiscard]] PixelFormat GetPixelFormat(VkFormat format);

			/**
			 * Get the pipeline stage flags from access flags.
			 *
			 * @param flags Access flags.
			 * @return The stage flags.
			 */
			[[nodiscard]] VkPipelineStageFlags GetPipelineStageFlags(VkAccessFlags flags);

			/**
			 * Find the best supported format available.
			 *
			 * @param engine The Vulkan engine.
			 * @param candidates The format candidates.
			 * @param tiling The image tiling.
			 * @param features The format features.
			 *
			 */
			[[nodiscard]] VkFormat FindSupportedFormat(const VulkanEngine& engine, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

			/**
			 * Check if a format has the stencil component.
			 *
			 * @param format The format to check.
			 */
			[[nodiscard]] bool HasStencilComponent(VkFormat format);

			/**
			 * Find the best depth format.
			 *
			 * @param engine The Vulkan engine.
			 */
			[[nodiscard]] VkFormat FindDepthFormat(const VulkanEngine& engine);
		}
	}
}