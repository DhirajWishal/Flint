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
		class VulkanVertexStorage final : public Core::VertexStorage
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

		public:
			/**
			 * Get the buffer containing positional data.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] VulkanBuffer* getPositionBuffer() { return m_Positions.get(); }

			/**
			 * Get the buffer containing positional data.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] const VulkanBuffer* getPositionBuffer() const { return m_Positions.get(); }


			/**
			 * Get the buffer containing normal data.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] VulkanBuffer* getNormalBuffer() { return m_Normals.get(); }

			/**
			 * Get the buffer containing normal data.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] const VulkanBuffer* getNormalBuffer() const { return m_Normals.get(); }

			/**
			 * Get the buffer containing texture coordinate data.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] VulkanBuffer* getTextureCoordinateBuffer() { return m_TextureCoordinates.get(); }

			/**
			 * Get the buffer containing texture coordinate data.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] const VulkanBuffer* getTextureCoordinateBuffer() const { return m_TextureCoordinates.get(); }

			/**
			 * Get the buffer containing color data.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] VulkanBuffer* getColorBuffer() { return m_Colors.get(); }

			/**
			 * Get the buffer containing color data.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] const VulkanBuffer* getColorBuffer() const { return m_Colors.get(); }

		private:
			std::unique_ptr<VulkanBuffer> m_Positions = nullptr;
			std::unique_ptr<VulkanBuffer> m_Normals = nullptr;
			std::unique_ptr<VulkanBuffer> m_TextureCoordinates = nullptr;
			std::unique_ptr<VulkanBuffer> m_Colors = nullptr;
		};
	}
}