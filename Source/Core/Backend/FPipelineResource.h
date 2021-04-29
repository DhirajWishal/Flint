// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FPipeline.h"

namespace Flint
{
	/**
	 * Flint pipeline resource object.
	 * This contains the resources required to submit uniforms and images to the draw call.
	 */
	class FPipelineResource : public BackendObject, public std::enable_shared_from_this<FPipelineResource> {

		/**
		 * Draw data structure.
		 * This structure contains the final information to be drawn.
		 */
		struct DrawData {
			DrawData() = default;
			DrawData(UI64 vertexCount, UI64 vertexOffset, UI64 indexCount, UI64 indexOffset, FDynamicStateContainer dynamicStates)
				: mVertexCount(vertexCount), mVertexOffset(vertexOffset), mIndexCount(indexCount), mIndexOffset(indexOffset), mDynamicStates(dynamicStates) {}

			FDynamicStateContainer mDynamicStates = {};

			UI64 mVertexCount = 0;
			UI64 mVertexOffset = 0;

			UI64 mIndexCount = 0;
			UI64 mIndexOffset = 0;
		};

	public:
		FPipelineResource(std::shared_ptr<FPipeline> pPipeline) : pPipeline(pPipeline) {}

		/**
		 * Register uniform buffers to the resource object.
		 * All the uniform buffers are to be registered ONCE.
		 *
		 * @param uniformBuffers: The uniform buffers to be registered.
		 */
		virtual void RegisterUniformBuffers(const UniformBufferContainer& uniformBuffers) = 0;

		/**
		 * Register uniform images to the resource object.
		 * All the uniform images are to be registered ONCE.
		 *
		 * @param unformImages: The uniform images to be registered.
		 */
		virtual void RegisterUniformImages(const UniformImageContainer& unformImages) = 0;

		/**
		 * Get the pipeline which the resource is bound to.
		 *
		 * @return The pipeline pointer.
		 */
		std::shared_ptr<FPipeline> GetPipeline() const { return pPipeline; }

		/**
		 * Add draw data to the pipeline resource.
		 *
		 * @param vertexCount: The number of vertexes to draw.
		 * @param vertexOffset: The vertex offset in the buffer.
		 * @param indexCount: The number of indexes to draw.
		 * @param indexOffset: The index offset in the buffer.
		 * @param dynamicStates: The dynamic states of the pipeline to be drawn.
		 * @return Draw data ID.
		 */
		UI64 AddDrawData(UI64 vertexCount, UI64 vertexOffset, UI64 indexCount, UI64 indexOffset, FDynamicStateContainer dynamicStates)
		{
			mDrawData[mIndex] = DrawData(vertexCount, vertexOffset, indexCount, indexOffset, dynamicStates);
			return mIndex++;
		}

		/**
		 * Get draw data from the resource.
		 *
		 * @return The draw data map.
		 */
		std::unordered_map<UI64, DrawData>& GetDrawData() { return mDrawData; }

		/**
		 * Get draw data from the resource.
		 *
		 * @return The draw data map.
		 */
		const std::unordered_map<UI64, DrawData> GetDrawData() const { return mDrawData; }

		/**
		 * Get the draw data index counter.
		 *
		 * @return The index.
		 */
		UI64 GetDrawDataIndex() const { return mIndex; }

	protected:
		std::unordered_map<UI64, DrawData> mDrawData;
		UI64 mIndex = 0;

		std::shared_ptr<FPipeline> pPipeline = nullptr;
	};
}