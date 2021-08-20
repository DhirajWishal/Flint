// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "RenderTarget.hpp"
#include "GraphicsPipeline.hpp"

namespace Flint
{
	RenderTarget::RenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList, UI32 threadCount)
		: DeviceBoundObject(pDevice), mExtent(extent), mBufferCount(bufferCount), pCommandBufferList(pCommandBufferList), mNumberOfThreads(threadCount), bThreadShouldRun(true)
	{
		if (extent.IsZero())
			FLINT_THROW_INVALID_ARGUMENT("Render target width and height should be greater than 0!");

		if (bufferCount == 0)
			FLINT_THROW_INVALID_ARGUMENT("Render target buffer count should be greater than 0!");

		if (!pCommandBufferList)
			FLINT_THROW_INVALID_ARGUMENT("Render target command buffer pointer should not be null!");

		InitiateThreads();
	}

	void RenderTarget::SubmitGraphicsPipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
	{
		bool inserted = false;
		for (UI32 i = 0; i < mDrawInstanceMaps.size(); i++)
		{
			DrawInstanceMap& instanceMap = mDrawInstanceMaps[i];
			if (instanceMap.find(pGeometryStore) != instanceMap.end())
			{
				INSERT_INTO_VECTOR(instanceMap[pGeometryStore], pPipeline);
				inserted = true;
				break;
			}
		}

		if (!inserted)
		{
			INSERT_INTO_VECTOR(mDrawInstanceMaps[mNextMap][pGeometryStore], pPipeline);
			INSERT_INTO_VECTOR(mDrawInstanceOrder[mNextMap], pGeometryStore);
			IncrementNextMap();
		}

		bIsAltered = true;
	}

	void RenderTarget::RemoveGraphicsPipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
	{
		for (UI32 i = 0; i < mDrawInstanceMaps.size(); i++)
		{
			DrawInstanceMap& instanceMap = mDrawInstanceMaps[i];
			if (instanceMap.find(pGeometryStore) != instanceMap.end())
			{
				std::vector<std::shared_ptr<GraphicsPipeline>>& pPipelines = instanceMap[pGeometryStore];
				for (UI64 j = 0; j < pPipelines.size(); j++)
				{
					if (pPipelines[j] == pPipeline)
					{
						pPipelines.erase(pPipelines.begin() + j);

						// Remove the geometry store if no pipeline is bound.
						if (pPipelines.empty())
						{
							instanceMap.erase(pGeometryStore);
							mDrawInstanceOrder[i].remove(pGeometryStore);
						}

						bIsAltered = true;
						return;
					}
				}
			}
		}
	}

	void RenderTarget::SubmitComputePipeline(const std::shared_ptr<ComputePipeline>& pPipeline, ComputeDispatchMode dispatchMode)
	{
		const auto pairToInsert = std::pair(pPipeline, dispatchMode);
		if (std::find(mComputePipelines.begin(), mComputePipelines.end(), pairToInsert) == mComputePipelines.end())
			FLINT_THROW_INVALID_ARGUMENT("Provided pipeline and dispatch mode already exists!");

		mComputePipelines.push_back(pairToInsert);
	}

	void RenderTarget::RemoveComputePipeline(const std::shared_ptr<ComputePipeline>& pPipeline, ComputeDispatchMode dispatchMode)
	{
		mComputePipelines.erase(std::find(mComputePipelines.begin(), mComputePipelines.end(), std::pair(pPipeline, dispatchMode)));
	}

	void RenderTarget::InitiateThreads()
	{
		if (mNumberOfThreads)
		{
			mDrawInstanceMaps.resize(mNumberOfThreads);
			mDrawInstanceOrder.resize(mNumberOfThreads);
		}
		else
		{
			mDrawInstanceMaps.resize(1);
			mDrawInstanceOrder.resize(1);
		}

		mBinarySemaphores.resize(mNumberOfThreads);

		for (UI32 i = 0; i < mNumberOfThreads; i++)
		{
			mWorkerThreads.push_back(std::thread([this](DrawInstanceMap& drawInstanceMap, std::list<std::shared_ptr<GeometryStore>>& drawOrder, BinarySemaphore& binarySemaphore, CountingSemaphore& countingSemaphore, std::atomic<bool>& shouldRun)
				{
					SecondaryCommandsWorker(drawInstanceMap, drawOrder, binarySemaphore, countingSemaphore, shouldRun);
				}, std::ref(mDrawInstanceMaps.at(i)), std::ref(mDrawInstanceOrder.at(i)), std::ref(mBinarySemaphores.at(i)), std::ref(mCountingSemaphore), std::ref(bThreadShouldRun)));
		}
	}

	void RenderTarget::TerminateThreads()
	{
		bThreadShouldRun = false;

		for (UI32 i = 0; i < mNumberOfThreads; i++)
			mWorkerThreads[i].join();
	}

	void RenderTarget::AcquireAllThreads()
	{
		mCountingSemaphore.Acquire(mNumberOfThreads);
	}

	void RenderTarget::ReleaseAllThreads()
	{
		for (UI32 i = 0; i < mNumberOfThreads; i++)
			mBinarySemaphores[i].Release();
	}

	void RenderTarget::IncrementNextMap()
	{
		mNextMap++;

		if (mNextMap >= mNumberOfThreads)
			mNextMap = 0;
	}
}