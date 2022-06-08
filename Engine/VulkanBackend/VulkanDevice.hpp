// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Device.hpp"
#include "Core/Types.hpp"
#include "Core/Containers/SparseArray.hpp"
#include "VulkanInstance.hpp"

#include <vk_mem_alloc.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBuffers;
		class VulkanGeometryStore;
		class VulkanBuffer;

		/**
		 * Vulkan engine class.
		 */
		class VulkanDevice final : public Core::Device<VulkanInstance>
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
			explicit VulkanDevice(VulkanInstance& instance);

			/**
			 * Destructor.
			 */
			~VulkanDevice() override;

			/**
			 * Wait till all the commands have finished execution.
			 */
			void waitIdle() override;

			/**
			 * Get the best depth pixel format.
			 *
			 * @return The pixel format.
			 */
			[[nodiscard]] Core::PixelFormat getBestDepthFormat() const override;

			/**
			 * Get the max supported multisample count.
			 *
			 * @return The multisample count.
			 */
			[[nodiscard]] Core::Multisample getMaximumMultisample() const override;

			/**
			 * Create a new buffer.
			 *
			 * @param size The size of the buffer.
			 * @param usage The buffer usage.
			 * @return The buffer handle.
			 */
			[[nodiscard]] Core::BufferHandle createBuffer(uint64_t size, Core::BufferUsage usage) override;

			/**
			 * Copy data to a buffer.
			 *
			 * @param handle The buffer handle.
			 * @param pData The data to copy.
			 * @param size The size of the data to copy.
			 * @param offset The buffer's offset to copy to.
			 */
			[[nodiscard]] void copyToBuffer(Core::BufferHandle handle, const std::byte* pData, uint64_t size, uint64_t offset = 0) override;

			/**
			 * Create a new texture image.
			 *
			 * @param path The path to the texture file.
			 * @param usage The image usage.
			 */
			[[nodiscard]] Core::ImageHandle createTextureImage(std::filesystem::path&& path, Core::ImageUsage usage) override;

		public:
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

			/**
			 * Get the Vulkan buffer using the handle.
			 *
			 * @return The buffer.
			 */
			[[nodiscard]] VulkanBuffer& getBuffer(Core::BufferHandle handle);

			/**
			 * Get the Vulkan buffer using the handle.
			 *
			 * @return The buffer.
			 */
			[[nodiscard]] const VulkanBuffer& getBuffer(Core::BufferHandle handle) const;

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

			SparseArray<VulkanBuffer, uint32_t> m_Buffers;

			VolkDeviceTable m_DeviceTable = {};

			std::vector<const char*> m_DeviceExtensions;

			VulkanCommandBuffers* m_pUtilityCommandBuffer = nullptr;

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
			[[nodiscard]] VkSampleCountFlagBits GetSampleCountFlagBits(Core::Multisample multisample);

			/**
			 * Get the Vulkan image format from pixel format.
			 *
			 * @param format The pixel format.
			 * @return The Vulkan format.
			 */
			[[nodiscard]] VkFormat GetImageFormat(Core::PixelFormat format);

			/**
			 * Get the pixel format from the Vulkan image format.
			 *
			 * @param format The Vulkan format.
			 * @return The pixel format.
			 */
			[[nodiscard]] Core::PixelFormat GetPixelFormat(VkFormat format);

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
			 * @param device The Vulkan engine.
			 * @param candidates The format candidates.
			 * @param tiling The image tiling.
			 * @param features The format features.
			 *
			 */
			[[nodiscard]] VkFormat FindSupportedFormat(const VulkanDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

			/**
			 * Check if a format has the stencil component.
			 *
			 * @param format The format to check.
			 */
			[[nodiscard]] bool HasStencilComponent(VkFormat format);

			/**
			 * Find the best depth format.
			 *
			 * @param device The Vulkan engine.
			 */
			[[nodiscard]] VkFormat FindDepthFormat(const VulkanDevice& device);

			/**
			 * Get the descriptor type.
			 *
			 * @param type The flint descriptor type.
			 * @return The Vulkan descriptor type.
			 */
			[[nodiscard]] VkDescriptorType GetDescriptorType(Core::ResourceType type);

			/**
			 * Get the size from a format.
			 *
			 * @param format The format to get the size of.
			 * @return The byte size.
			 */
			[[nodiscard]] uint8_t GetSizeFromFormat(VkFormat format);

			/**
			 * Get the format from the attribute type.
			 *
			 * @param type The type of the attribute.
			 * @return The Vulkan format.
			 */
			[[nodiscard]] VkFormat GetVkFormat(Flint::Core::DataType type);
		}
	}
}