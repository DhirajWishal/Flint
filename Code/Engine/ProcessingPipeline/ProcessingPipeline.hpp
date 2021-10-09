// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "Nodes/ProcessingNode.hpp"

namespace Flint
{
	/**
	 * Flint processing pipeline class.
	 * This object is used to carry out processes. These are used to render content to a display.
	 *
	 * The processing pipeline comprises of multiple command pipelines. These command pipelines are used to pass commands to the GPU device.
	 * Then there are nodes. Each processing node acts as a render stage. Each of these nodes can request a command buffer allocator and can use
	 * multi threading to bind resources, and perform a certain task. The processing pipeline can contain multiple nodes for rendering.
	 *
	 * The client application can derive from this object to extend the features of this object.
	 */
	class ProcessingPipeline
	{
	public:
		/**
		 * Create the processing pipeline.
		 *
		 * @param pDevice The device pointer.
		 * @param frameExtent The size of the frame.
		 * @param displayTitle The title of the display.
		 * @param pipelineCount The pipeline count to use. If set to 0, the best supported is set.
		 * @param msaaCount The number of MSAA samples to use. If the MSAA count is greater than One, a color buffer is used.
		 * @param forceColorBuffer Force to use a color buffer. Default is false.
		 * @param forceDepthBuffer Whether or not to use a depth buffer. Default is false.
		 */
		ProcessingPipeline(const std::shared_ptr<Device>& pDevice, const FBox2D frameExtent, const std::string& displayTitle, const UI32 pipelineCount, const MultiSampleCount msaaCount, const bool forceColorBuffer = false, const bool forceDepthBuffer = false);

		/**
		 * Destructor.
		 */
		virtual ~ProcessingPipeline();

		/**
		 * Create a new processing node.
		 * This function will create a new processing node and will add the node to the processing nodes array.
		 *
		 * @param args The constructor arguments.
		 * @return The created object shared pointer.
		 */
		template<class Type, class... Args>
		std::shared_ptr<Type> CreateProcessingNode(const Args&... args)
		{
			static_assert(!std::is_base_of_v<Type, ProcessingNode>, "'Type' should be derived from 'ProcessingNode'!");

			auto pNode = std::make_shared<Type>(this, args...);
			pProcessingNodes.push_back(pNode);
			return pNode;
		}

		/**
		 * Execute everything in the current pipeline.
		 * This method can also be overridden by the client.
		 */
		virtual void Execute();

	public:
		/**
		 * Get the set multi sample count.
		 *
		 * @return The multi sample count.
		 */
		MultiSampleCount GetMultiSampleCount() const { return mMultiSampleCount; }

		/**
		 * Get the extent of the pipeline.
		 *
		 * @return The extent.
		 */
		FBox2D GetExtent() const { return pScreenBoundRenderTarget->GetExtent(); }

		/**
		 * Get the buffer count of the pipeline.
		 *
		 * @return The buffer count.
		 */
		UI32 GetBufferCount() const { return pScreenBoundRenderTarget->GetBufferCount(); }

		/**
		 * Get the device to which the pipeline is bound to.
		 *
		 * @return The device pointer.
		 */
		std::shared_ptr<Device> GetDevice() const { return pScreenBoundRenderTarget->GetDevice(); }

		/**
		 * Get the display pointer.
		 *
		 * @reutrn The display pointer.
		 */
		std::shared_ptr<Display> GetDisplay() const { return pDisplay; }

		/**
		 * Check if the color image is present.
		 *
		 * @return The boolean value.
		 */
		bool IsColorImagePresent() const { return bContainsColorBuffer; }

		/**
		 * Check if the depth image is present.
		 *
		 * @return The boolean value.
		 */
		bool IsDepthImagePresent() const { return bContainsDepthBuffer; }

		/**
		 * Get the color image from the pipeline.
		 * If the color image is not present, it returns a nullptr.
		 *
		 * @return The color image pointer.
		 */
		std::shared_ptr<Image> GetColorBuffer() const;

		/**
		 * Get the depth image from the pipeline.
		 * If the depth image is not present, it returns a nullptr.
		 *
		 * @return The depth image pointer.
		 */
		std::shared_ptr<Image> GetDepthBuffer() const;

		/**
		 * Get the screen bound render target pointer.
		 *
		 * @retrun The render target pointer.
		 */
		std::shared_ptr<ScreenBoundRenderTarget> GetScreenBoundRenderTarget() const { return pScreenBoundRenderTarget; }

		/**
		 * Get the command buffer allocator pointer.
		 *
		 * @return The command buffer allocator pointer.
		 */
		std::shared_ptr<CommandBufferAllocator> GetCommandBufferAllocator() const { return pCommandBufferAllocator; }

		/**
		 * Get the command buffers.
		 *
		 * @return The vector of command buffer pointers.
		 */
		std::vector<std::shared_ptr<CommandBuffer>> GetTheCommandBuffers() const { return pCommandBuffers; }

		/**
		 * Get the current frame index.
		 *
		 * @return The frame index.
		 */
		UI32 GetCurrentFrameIndex() const { return pScreenBoundRenderTarget->GetFrameIndex(); }

		/**
		 * Get the current image index.
		 *
		 * @return The image index.
		 */
		UI32 GetCurrentImageIndex() const { return pScreenBoundRenderTarget->GetImageIndex(); }

		/**
		 * Get the current command buffer in flight.
		 *
		 * @return The command buffer in flight.
		 */
		std::shared_ptr<CommandBuffer> GetCommandBufferInFlight() const { return pCommandBuffers[GetCurrentFrameIndex()]; }

		/**
		 * Get the synchronization primitives of the pipeline.
		 *
		 * @return The vector of synchronization primitives.
		 */
		std::vector<std::shared_ptr<SynchronizationPrimitive>> GetSynchronizationPrimitives() const { return pSynchronizationPrimitives; }

		/**
		 * Get the synchronization primitive that is in flight.
		 *
		 * @return The synchronization primitive.
		 */
		std::shared_ptr<SynchronizationPrimitive> GetInFlightSynchronizationPrimitive() const { return pSynchronizationPrimitives[GetCurrentFrameIndex()]; }

		/**
		 * Get all the processing nodes in the pipeline.
		 *
		 * @return The processing node vector.
		 */
		std::vector<std::shared_ptr<ProcessingNode>> GetProcessingNodes() const { return pProcessingNodes; }

	protected:
		std::vector<std::shared_ptr<ProcessingNode>> pProcessingNodes = {};
		std::vector<std::shared_ptr<CommandBuffer>> pCommandBuffers = {};
		std::vector<std::shared_ptr<SynchronizationPrimitive>> pSynchronizationPrimitives = {};

		std::shared_ptr<Display> pDisplay = nullptr;

		std::shared_ptr<ScreenBoundRenderTarget> pScreenBoundRenderTarget = nullptr;
		std::shared_ptr<CommandBufferAllocator> pCommandBufferAllocator = nullptr;

		MultiSampleCount mMultiSampleCount = MultiSampleCount::One;

		bool bContainsColorBuffer = false;
		bool bContainsDepthBuffer = false;
	};
}
