// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBufferList.hpp"
#include "Core/CountingSemaphore.hpp"

#include <unordered_map>

namespace Flint
{
	class GraphicsPipeline;
	class GeometryStore;

	using DrawInstanceMap = std::unordered_map<std::shared_ptr<GeometryStore>, std::vector<std::shared_ptr<GraphicsPipeline>>>;

	/**
	 * Flint render target object.
	 * This object is the base class for all the supported render targets.
	 *
	 * Render target objects are used to submit data to the device to be rendered or for compute purposes.
	 *
	 * Render targets can use secondary threads to speed up rendering. By default, the number of threads used per render target is 0, though it can be set to a specified value.
	 * Load balancing is done automatically by the render target using the following equation.
	 *		Load Per Thread = Draw Instances / Thread Count
	 * Each thread contains its own pair of GeometryStore and pipelines. So we recommend having one thread per geometry store in use IF the pipeline count or draw data count is high.
	 */
	class RenderTarget : public DeviceBoundObject
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 * @param extent: The render target extent.
		 * @param bufferCount: The frame buffer count.
		 * @param pCommandBufferList: The command buffer list used by the render target.
		 * @param threadCount: The number of threads used for secondary commands
		 */
		RenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList, UI32 threadCount = 0);

		/**
		 * Get the render targets clear screen color.
		 *
		 * @return The color container.
		 */
		virtual FColor4D GetClearColor() const = 0;

		/**
		 * Set the clear color values.
		 *
		 * @param newColor: The new color to set.
		 */
		virtual void SetClearColor(const FColor4D& newColor) = 0;

	public:
		/**
		 * Submit a pipeline to the render target to be drawn.
		 *
		 * @param pGeometryStore: The geometry store to bind to.
		 * @param pPipeline: The pipeline to submit.
		 */
		void SubmitPipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline);

		/**
		 * Remove a pipeline from the render target.
		 *
		 * @param pGeometryStore: The geometry store which the pipeline is bound to.
		 * @param pPipeline: The pipeline to remove.
		 */
		void RemovePipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline);

	public:
		/**
		 * Get the command buffer list of the render target.
		 *
		 * @return The command buffer list pointer.
		 */
		const std::shared_ptr<CommandBufferList> GetCommandBufferList() const { return pCommandBufferList; }

		/**
		 * Get the render target extent.
		 *
		 * @return The extent.
		 */
		const FBox2D GetExtent() const { return mExtent; }

		/**
		 * Get the render target buffer count.
		 *
		 * @return The buffer count.
		 */
		const UI32 GetBufferCount() const { return mBufferCount; }

		/**
		 * Get the number of threads used by this thread.
		 *
		 * @return The thread count.
		 */
		const UI32 GetNumberOfThreads() const { return mNumberOfThreads; }

		/**
		 * Check if the render target is altered.
		 *
		 * @return Boolean value.
		 */
		const bool IsAltered() const { return bIsAltered; }

		/**
		 * Flag the render target that an alteration is made.
		 */
		void FlagAltered() { bIsAltered = true; }

	protected:
		/**
		 * Increment the frame index.
		 * If the frame index is bigger than or equal to the buffer count, it is rolled back to 0.
		 */
		void IncrementFrameIndex() { mFrameIndex++; if (mFrameIndex >= mBufferCount) mFrameIndex = 0; }

		/**
		 * Bind the volatile instances.
		 */
		virtual void BindVolatileInstances() = 0;

		/**
		 * Secondary commands worker function.
		 * This function will be executed as a worker thread.
		 *
		 * @param drawInstanceMap: The draw instance map.
		 * @param drawOrder: The order in which resources are drawn.
		 * @param binarySemaphore: The binary semaphore used to control the secondary thread.
		 * @param countingSemaphore: The counting semaphore used to control the parent thread.
		 * @param shouldRun: The boolean stating whether or not to run.
		 */
		virtual void SecondaryCommandsWorker(DrawInstanceMap& drawInstanceMap, std::list<std::shared_ptr<GeometryStore>>& drawOrder, BinarySemaphore& binarySemaphore, CountingSemaphore& countingSemaphore, std::atomic<bool>& shouldRun) = 0;

		/**
		 * Initiate all the worker threads.
		 */
		void InitiateThreads();

		/**
		 * Terminate all the threads.
		 */
		void TerminateThreads();

		/**
		 * Acquire all the threads.
		 * This waits until all the threads are acquired.
		 */
		void AcquireAllThreads();

		/**
		 * Release all the threads.
		 */
		void ReleaseAllThreads();

	private:
		/**
		 * Increment the next map to place the next geometry data to.
		 */
		void IncrementNextMap();

	protected:
		std::shared_ptr<CommandBufferList> pCommandBufferList = nullptr;

		std::vector<DrawInstanceMap> mDrawInstanceMaps;
		std::vector<std::list<std::shared_ptr<GeometryStore>>> mDrawInstanceOrder;

		std::vector<std::thread> mWorkerThreads;
		std::vector<BinarySemaphore> mBinarySemaphores;
		std::atomic<bool> bThreadShouldRun;
		CountingSemaphore mCountingSemaphore = {};

		FBox2D mExtent = {};
		UI32 mBufferCount = 0;
		UI32 mFrameIndex = 0;

		UI32 mNumberOfThreads = 0;

		bool bIsAltered = false;

	private:
		UI32 mNextMap = 0;
	};
}