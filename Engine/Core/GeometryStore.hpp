// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.hpp"

namespace Flint
{
	/**
	 * Geometry store class.
	 * This object can store any number of geometry and is the preferred method of storing data.
	 */
	class GeometryStore : public EngineBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine reference.
		 */
		explicit GeometryStore(Engine& engine) : EngineBoundObject(engine) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~GeometryStore() = default;

		/**
		 * create new geometry.
		 *
		 * @param vertexSize The total vertex size to add.
		 * @param indexSize The total index size to add.
		 */
		[[nodiscard]] virtual Geometry createGeometry(uint32_t vertexSize, uint32_t indexSize) = 0;

		/**
		 * Map the vertex data from the device to the application.
		 *
		 * @param size The size of the mapped data.
		 * @param offset The offset of the buffer. Default is 0.
		 * @return The mapped memory.
		 */
		[[nodiscard]] virtual std::byte* mapVertexData(uint64_t size, uint64_t offset = 0) = 0;

		/**
		 * Unmap the mapped vertex data.
		 */
		virtual void unmapVertexData() = 0;

		/**
		 * Map the index data from the device to the application.
		 *
		 * @param size The size of the mapped data.
		 * @param offset The offset of the buffer. Default is 0.
		 * @return The mapped memory.
		 */
		[[nodiscard]] virtual std::byte* mapIndexData(uint64_t size, uint64_t offset = 0) = 0;

		/**
		 * Unmap the mapped index data.
		 */
		virtual void unmapIndexData() = 0;
	};
}