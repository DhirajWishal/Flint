// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"
#include "Material.hpp"

namespace Flint
{
	/**
	 * Flint wire frame object.
	 * Wire frame contains information about a single draw instance, and its material data.
	 */
	class WireFrame
	{
	public:
		/**
		 * Default constructor.
		 */
		WireFrame() = default;

		/**
		 * Construct the wire frame.
		 *
		 * @param name The name of the wire frame.
		 * @param vertexOffset The vertex offset of the wire frame.
		 * @param vertexCount The number of vertexes in the wire frame.
		 * @param indexOffset The index offset of the wire frame.
		 * @param indexCount The number of indexes in the wire frame.
		 */
		WireFrame(const std::string& name, uint64_t vertexOffset, uint64_t vertexCount, uint64_t indexOffset, uint64_t indexCount);

		/**
		 * Construct the wire frame.
		 *
		 * @param name The name of the wire frame.
		 * @param vertexOffset The vertex offset of the wire frame.
		 * @param vertexCount The number of vertexes in the wire frame.
		 * @param indexOffset The index offset of the wire frame.
		 * @param indexCount The number of indexes in the wire frame.
		 * @param material The material of the wire frame.
		 */
		WireFrame(const std::string& name, uint64_t vertexOffset, uint64_t vertexCount, uint64_t indexOffset, uint64_t indexCount, const Material& material);

	public:
		/**
		 * Get the name of the wire frame.
		 *
		 * @return The name string.
		 */
		const std::string GetName() const { return mName; }

		/**
		 * Get the material of the wire frame.
		 * 
		 * @return Material reference.
		 */
		const Material GetMaterial() const { return mMaterial; }

		/**
		 * Get the material of the wire frame.
		 *
		 * @return Material reference.
		 */
		Material& GetMaterial() { return mMaterial; }

		/**
		 * Get the vertex offset of the wire frame.
		 *
		 * @return The vertex offset.
		 */
		const uint64_t GetVertexOffset() const { return mVertexOffset; }

		/**
		 * Set the vertex offset.
		 *
		 * @param offset The offset to set.
		 */
		void SetVertexOffset(const uint64_t offset) { mVertexOffset = offset; }

		/**
		 * Get the vertex count of the wire frame.
		 *
		 * @return The vertex count.
		 */
		const uint64_t GetVertexCount() const { return mVertexCount; }

		/**
		 * Get the index offset of the wire frame.
		 *
		 * @return The index offset.
		 */
		const uint64_t GetIndexOffset() const { return mIndexOffset; }

		/**
		 * Set the index offset.
		 *
		 * @param offset The offset to set.
		 */
		void SetIndexOffset(const uint64_t offset) { mIndexOffset = offset; }

		/**
		 * Get the index count of the wire frame.
		 *
		 * @return The index count.
		 */
		const uint64_t GetIndexCount() const { return mIndexCount; }

	private:
		std::string mName = "";
		Material mMaterial = {};

		uint64_t mVertexOffset = 0, mVertexCount = 0;
		uint64_t mIndexOffset = 0, mIndexCount = 0;
	};
}