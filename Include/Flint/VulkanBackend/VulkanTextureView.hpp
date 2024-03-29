// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Backend/TextureView.hpp"
#include "VulkanTexture2D.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Vulkan texture view class.
		 */
		class VulkanTextureView final : public TextureView
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pTexture The texture pointer.
			 */
			explicit VulkanTextureView(const std::shared_ptr<VulkanDevice>& pDevice, const std::shared_ptr<VulkanTexture2D>& pTexture);

			/**
			 * Destructor.
			 */
			~VulkanTextureView() override;

			/**
			 * Terminate this object.
			 */
			void terminate() override;

			/**
			 * Get the view handle.
			 *
			 * @return The Vulkan handle.
			 */
			[[nodiscard]] VkImageView getViewHandle() const { return m_ImageView; }

		private:
			VkImageView m_ImageView = VK_NULL_HANDLE;
		};
	}
}