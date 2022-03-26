// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/CommandBuffer.hpp"
#include "GraphicsCore/Display.hpp"

namespace Flint
{
	class ProcessingPipeline;

	/**
	 * Flint processing node.
	 * Processing nodes are used to process resources in a single processing pipeline.
	 */
	class ProcessingNode : public FObject
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pProcessingPipeline The processing pipeline pointer.
		 */
		ProcessingNode(ProcessingPipeline* pProcessingPipeline);

		/**
		 * Process the node.
		 * This is used to perform whatever the process the node does.
		 *
		 * @param pPreviousNode The node that was executed before this.
		 * @param pCommandBuffer The current command buffer pointer.
		 * @param frameIndex The current frame index.
		 * @param imageIndex The current image index.
		 */
		virtual void Process(ProcessingNode* pPreviousNode, const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 frameIndex, const uint32 imageIndex) = 0;

	public:
		/**
		 * Get the set multi sample count.
		 *
		 * @return The multi sample count.
		 */
		MultiSampleCount GetMultiSampleCount() const;

		/**
		 * Get the extent of the pipeline.
		 *
		 * @return The extent.
		 */
		FBox2D GetExtent() const;

		/**
		 * Get the buffer count of the pipeline.
		 *
		 * @return The buffer count.
		 */
		uint32 GetBufferCount() const;

		/**
		 * Get the device to which the pipeline is bound to.
		 *
		 * @return The device pointer.
		 */
		std::shared_ptr<Device> GetDevice() const;

		/**
		 * Get the display pointer.
		 *
		 * @reutrn The display pointer.
		 */
		std::shared_ptr<Display> GetDisplay() const;

	protected:
		ProcessingPipeline* pProcessingPipeline = nullptr;
	};
}