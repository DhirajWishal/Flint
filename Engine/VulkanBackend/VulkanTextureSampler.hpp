// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/TextureSampler.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan texture sampler class.
		 */
		class VulkanTextureSampler final : public TextureSampler
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The sampler specification.
			 */
			explicit VulkanTextureSampler(const std::shared_ptr<VulkanDevice>& pDevice, TextureSamplerSpecification&& specification);

			/**
			 * Destructor.
			 */
			~VulkanTextureSampler() override;

			/**
			 * Destroy this object.
			 */
			void terminate() override;

			/**
			 * Get the sampler handle.
			 *
			 * @return The Vulkan sampler.
			 */
			[[nodiscard]] VkSampler getSamplerHandle() const { return m_Sampler; }

		private:
			VkSampler m_Sampler = VK_NULL_HANDLE;
		};
	}
}