// Copyright 2021 Dhiraj Wishal
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
		WireFrame(const std::string& name, UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount);

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
		WireFrame(const std::string& name, UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount, const Material& material);

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
		const UI64 GetVertexOffset() const { return mVertexOffset; }

		/**
		 * Set the vertex offset.
		 *
		 * @param offset The offset to set.
		 */
		void SetVertexOffset(const UI64 offset) { mVertexOffset = offset; }

		/**
		 * Get the vertex count of the wire frame.
		 *
		 * @return The vertex count.
		 */
		const UI64 GetVertexCount() const { return mVertexCount; }

		/**
		 * Get the index offset of the wire frame.
		 *
		 * @return The index offset.
		 */
		const UI64 GetIndexOffset() const { return mIndexOffset; }

		/**
		 * Set the index offset.
		 *
		 * @param offset The offset to set.
		 */
		void SetIndexOffset(const UI64 offset) { mIndexOffset = offset; }

		/**
		 * Get the index count of the wire frame.
		 *
		 * @return The index count.
		 */
		const UI64 GetIndexCount() const { return mIndexCount; }

	private:
		std::string mName = "";
		Material mMaterial = {};

		UI64 mVertexOffset = 0, mVertexCount = 0;
		UI64 mIndexOffset = 0, mIndexCount = 0;
	};
}