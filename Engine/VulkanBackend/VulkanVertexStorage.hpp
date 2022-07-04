// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/VertexStorage.hpp"

#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan vertex storage class.
		 */
		class VulkanVertexStorage final : public VertexStorage
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device reference.
			 */
			explicit VulkanVertexStorage(const std::shared_ptr<VulkanDevice>& pDevice);

			/**
			 * Destructor.
			 */
			~VulkanVertexStorage() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

			/**
			 * Insert data to the vertex storage.
			 *
			 * @param attribute The attribute type of the data.
			 * @param pStaggingBuffer The stagging buffer pointer to copy the data from.
			 * @return The offset at which the newly inserted data are stored.
			 */
			[[nodiscard]] uint64_t insert(VertexAttribute attribute, const Buffer* pStaggingBuffer) override;

		public:
			/**
			 * Get the buffer containing required data.
			 *
			 * @param attribute The attribute to access.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] VulkanBuffer* getBuffer(VertexAttribute attribute) { return m_pBuffers[EnumToInt(attribute)].get(); }

			/**
			 * Get the buffer containing required data.
			 *
			 * @param attribute The attribute to access.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] const VulkanBuffer* getBuffer(VertexAttribute attribute) const { return m_pBuffers[EnumToInt(attribute)].get(); }

		private:
			std::array<std::shared_ptr<VulkanBuffer>, EnumToInt(VertexAttribute::Max)> m_pBuffers;
		};
	}
}