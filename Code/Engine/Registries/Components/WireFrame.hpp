// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"

#include <vector>
#include <string>

namespace Flint
{
	/**
	 * Flint wire frame object.
	 * This object contains information about a single model, with multiple draw segments (sub-meshes).
	 */
	class WireFrame
	{
	public:
		/**
		 * Draw Segment structure.
		 */
		struct DrawSegment
		{
			DrawSegment() = default;
			DrawSegment(const std::string& name, UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount)
				: mSegmentName(name), mVertexOffset(vertexCount), mVertexCount(vertexCount), mIndexOffset(indexOffset), mIndexCount(indexCount) {}

			std::string mSegmentName = "";

			UI64 mVertexOffset = 0;
			UI64 mVertexCount = 0;
			UI64 mIndexOffset = 0;
			UI64 mIndexCount = 0;
		};

	public:
		WireFrame() = default;
		WireFrame(UI64 storageID, const std::string& wireFrameName, const std::vector<DrawSegment>& segments) : mStorageID(storageID), mWireFrameName(wireFrameName), mSegments(segments) {}

		/**
		 * Set the wire frame name.
		 *
		 * @param name: The wire frame name.
		 */
		void SetName(const std::string& name) { mWireFrameName = name; }

		/**
		 * Get the wire frame name.
		 *
		 * @return The name string.
		 */
		const std::string_view GetName() const { return mWireFrameName; }

		/**
		 * Get the segments stored.
		 *
		 * @return The segments vector.
		 */
		const std::vector<DrawSegment> GetSegments() const { return mSegments; }

		/**
		 * Get the storage ID of the wire frame.
		 *
		 * @return The storage ID.
		 */
		const UI64 GetStorageID() const { return mStorageID; }

	private:
		std::vector<DrawSegment> mSegments = {};
		std::string mWireFrameName = "";

		UI64 mStorageID = 0;
	};
}