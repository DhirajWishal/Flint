// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "RenderTarget.hpp"
#include "GraphicsPipeline.hpp"

namespace Flint
{
	RenderTarget::RenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList, UI32 threadCount)
		: DeviceBoundObject(pDevice), mExtent(extent), mBufferCount(bufferCount), pCommandBufferList(pCommandBufferList), pVolatileCommandBufferList(pCommandBufferList->CreateChild()), mNumberOfThreads(threadCount), bThreadShouldRun(true)
	{
		if (extent.IsZero())
			FLINT_THROW_INVALID_ARGUMENT("Render target width and height should be greater than 0!");

		if (bufferCount == 0)
			FLINT_THROW_INVALID_ARGUMENT("Render target buffer count should be greater than 0!");

		if (!pCommandBufferList)
			FLINT_THROW_INVALID_ARGUMENT("Render target command buffer pointer should not be null!");

		InitiateThreads();
	}

	void RenderTarget::SubmitPipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
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

	void RenderTarget::RemovePipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
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

	void RenderTarget::SubmitVolatilePipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
	{
		if (mVolatileDrawInstanceMap.find(pGeometryStore) != mVolatileDrawInstanceMap.end())
			INSERT_INTO_VECTOR(mVolatileDrawInstanceMap[pGeometryStore], pPipeline);
		else
		{
			mVolatileDrawInstanceMap[pGeometryStore] = { pPipeline };
			mVolatileDrawInstanceOrder.push_back(pGeometryStore);
		}

		bIsAltered = true;
	}

	void RenderTarget::RemoveVolatilePipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
	{
		if (mVolatileDrawInstanceMap.find(pGeometryStore) == mVolatileDrawInstanceMap.end())
			return;

		std::vector<std::shared_ptr<GraphicsPipeline>>& pPipelines = mVolatileDrawInstanceMap[pGeometryStore];
		for (UI64 j = 0; j < pPipelines.size(); j++)
		{
			if (pPipelines[j] == pPipeline)
			{
				pPipelines.erase(pPipelines.begin() + j);

				// Remove the geometry store if no pipeline is bound.
				if (pPipelines.empty())
				{
					mVolatileDrawInstanceMap.erase(pGeometryStore);
					mVolatileDrawInstanceOrder.remove(pGeometryStore);
				}

				bIsAltered = true;
				return;
			}
		}
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

		mWorkerThreads.resize(mNumberOfThreads);
		mBinarySemaphores.resize(mNumberOfThreads);

		for (UI32 i = 0; i < mNumberOfThreads; i++)
			mWorkerThreads[i] = std::thread([this, i] { SecondaryCommandsWorker(mDrawInstanceMaps[i], mDrawInstanceOrder[i], mBinarySemaphores[i], mCountingSemaphore, bThreadShouldRun); });
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