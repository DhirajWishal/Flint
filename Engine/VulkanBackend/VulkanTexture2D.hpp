// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Texture2D.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBuffers;

		/**
		 * Vulkan texture 2D class.
		 */
		class VulkanTexture2D final : public std::enable_shared_from_this<VulkanTexture2D>, public Texture2D
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param width The width of the texture.
			 * @param height The height of the texture.
			 * @param usage The texture usage.
			 * @param format The texture's pixel format.
			 * @param mipLevels The texture's mip levels. If this is set to 0 (default), the best value is set.
			 * @param multisampleCount The multisample count of the image. Default is one.
			 * @param pDataStore The data store pointer to load the data from. Default is nullptr.
			 */
			explicit VulkanTexture2D(const std::shared_ptr<VulkanDevice>& pDevice, uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format, uint32_t mipLevels = 0, Multisample multisampleCount = Multisample::One, const std::byte* pDataStore = nullptr);

			/**
			 * Destructor.
			 */
			~VulkanTexture2D() override;

			/**
			 * Terminate this object.
			 */
			void terminate() override;

			/**
			 * Create the image view for the current texture.
			 *
			 * @return The texture view.
			 */
			[[nodiscard]] std::shared_ptr<TextureView> createView() override;

			/**
			 * Copy the texture image to a buffer.
			 *
			 * @return The staging buffer containing the image.
			 */
			[[nodiscard]] std::shared_ptr<Buffer> toBuffer() const override;

			/**
			 * Copy the image data from a buffer.
			 *
			 * @param pBuffer The buffer to load the data from.
			 */
			void copyFrom(const Buffer* pBuffer) override;

			/**
			 * Copy the image data from a raw memory pointer.
			 * Make sure that the memory size is the same as the image size (width * height * pixel size).
			 *
			 * @param pDataStore The data store to load the data from.
			 */
			void copyFrom(const std::byte* pDataStore) override;

			/**
			 * Copy the image from another texture.
			 *
			 * @param pTexture The texture object pointer to copy from.
			 */
			void copyFrom(const Texture* pTexutre) override;

			/**
			 * Get the image handle.
			 *
			 * @return The Vulkan handle.
			 */
			[[nodiscard]] VkImage getImageHandle() const { return m_Image; };

		private:
			/**
			 * Copy the texture image to a buffer.
			 *
			 * @param commandBuffers The command buffers to record the commands to.
			 * @return The staging buffer containing the image.
			 */
			[[nodiscard]] std::shared_ptr<Buffer> toBufferBatched(const VulkanCommandBuffers& commandBuffers) const;

			/**
			 * Copy the image data from a buffer.
			 *
			 * @param commandBuffers The command buffers to record the commands to.
			 * @param pBuffer The buffer to load the data from.
			 */
			void copyFromBatched(const VulkanCommandBuffers& commandBuffers, const Buffer* pBuffer);

			/**
			 * Generate the mipmaps if required.
			 */
			void generateMipMaps();

		private:
			/**
			 * Create the image and allocator.
			 */
			void createImageAndAllocator();

		private:
			VkImage m_Image = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;

			VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		};
	}
}