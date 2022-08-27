// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TextureView.hpp"
#include "TextureSampler.hpp"

#include <variant>
#include <unordered_map>

namespace Flint
{
	namespace Backend
	{
		/**
		 * Mesh binding table class.
		 * This is used to bind resources to different resource locations.
		 */
		class MeshBindingTable final
		{
			/**
			 * Image binding structure.
			 * This is a helper structure to bind image samplers.
			 */
			struct ImageBinding final
			{
				std::shared_ptr<TextureView> m_pTextureView;
				std::shared_ptr<TextureSampler> m_pTextureSampler;
				ImageUsage m_ImageUsage = ImageUsage::Graphics;
			};

		public:
			/**
			 * Default constructor
			 */
			MeshBindingTable() = default;

			/**
			 * Bind a buffer to the required binding.
			 *
			 * @param binding The buffer's binding.
			 * @param pBuffer The buffer to bind.
			 */
			void bind(uint32_t binding, const std::shared_ptr<Buffer>& pBuffer);

			/**
			 * Bind an image to the required binding.
			 *
			 * @param binding The buffer's binding.
			 * @param pView The image view to bind.
			 * @param pSampler The image sampler.
			 * @param currentUsage The current usage of the image. Note that it should only be either Graphics or Storage.
			 */
			void bind(uint32_t binding, const std::shared_ptr<TextureView>& pView, const std::shared_ptr<TextureSampler>& pSampler, ImageUsage currentUsage);

			/**
			 * Generate the hash for this table.
			 *
			 * @return The hash value for this table.
			 */
			[[nodiscard]] uint64_t generateHash() const;

			/**
			 * Get the bound buffers.
			 *
			 * @return The buffer map.
			 */
			[[nodiscard]] const std::unordered_map<uint32_t, std::shared_ptr<Buffer>>& getBuffers() const { return m_pBuffers; }

			/**
			 * Get the bound images.
			 *
			 * @return The image map.
			 */
			[[nodiscard]] const std::unordered_map<uint32_t, ImageBinding>& getImages() const { return m_Images; }

		private:
			std::unordered_map<uint32_t, std::shared_ptr<Buffer>> m_pBuffers;
			std::unordered_map<uint32_t, ImageBinding> m_Images;
		};
	}
}