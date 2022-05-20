// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/GeometryStore.hpp"
#include "VulkanEngine.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan geometry store class.
		 */
		class VulkanGeometryStore final : public GeometryStore
		{
			/**
			 * Buffer structure.
			 */
			struct Buffer final
			{
				VkBuffer m_Buffer = VK_NULL_HANDLE;
				VmaAllocation m_Allocation = nullptr;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param engine The engine reference.
			 */
			explicit VulkanGeometryStore(Engine& engine);

			/**
			 * Destructor.
			 */
			~VulkanGeometryStore() override;

			/**
			 * Create new geometry.
			 *
			 * @param vertexSize The total vertex size to add.
			 * @param indexSize The total index size to add.
			 */
			[[nodiscard]] Geometry createGeometry(uint32_t vertexSize, uint32_t indexSize) override;

			/**
			 * Map the vertex data from the device to the application.
			 *
			 * @param size The size of the mapped data.
			 * @param offset The offset of the buffer. Default is 0.
			 * @return The mapped memory.
			 */
			[[nodiscard]] std::byte* mapVertexData(uint64_t size, uint64_t offset = 0) override;

			/**
			 * Unmap the mapped vertex data.
			 */
			void unmapVertexData() override;

			/**
			 * Map the index data from the device to the application.
			 *
			 * @param size The size of the mapped data.
			 * @param offset The offset of the buffer. Default is 0.
			 * @return The mapped memory.
			 */
			[[nodiscard]] std::byte* mapIndexData(uint64_t size, uint64_t offset = 0) override;

			/**
			 * Unmap the mapped index data.
			 */
			void unmapIndexData() override;

		private:
			/**
			 * Extend the internal buffers.
			 *
			 * @param vertexSize The new vertex size to add.
			 * @param indexSize The new index size to add.
			 */
			void extendBuffers(uint32_t vertexSize, uint32_t indexSize);

			/**
			 * Create a new buffer.
			 *
			 * @param size The size of the buffer.
			 * @param usage The buffer usage.
			 */
			Buffer createBuffer(uint64_t size, VkBufferUsageFlags usage) const;

			/**
			 * Destroy a buffer.
			 *
			 * @param buffer The buffer to destroy.
			 */
			void destroyBuffer(Buffer buffer) const;

		private:
			Buffer m_VertexBuffer = {};
			Buffer m_IndexBuffer = {};

			uint64_t m_VertexSize = 0;
			uint64_t m_IndexSize = 0;
		};
	}
}