// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ScreenBoundRenderTarget.hpp"

namespace Flint
{
	ScreenBoundRenderTarget::ScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList, UI32 threadCount)
		: RenderTarget(pDevice, extent, bufferCount, pCommandBufferList, threadCount), pVolatileCommandBufferList(pCommandBufferList->CreateChild()), pDisplay(pDisplay)
	{
		if (!pDisplay)
			FLINT_THROW_INVALID_ARGUMENT("Screen bound render target device pointer should not be null!");
	}

	void ScreenBoundRenderTarget::SubmitVolatilePipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
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

	void ScreenBoundRenderTarget::RemoveVolatilePipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
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

	void ScreenBoundRenderTarget::AttachOffScreenRenderTarget(const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget)
	{
		pOffScreenRenderTargets.push_back(pOffScreenRenderTarget);
	}

	void ScreenBoundRenderTarget::DetachOffScreenRenderTarget(const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget)
	{
		pOffScreenRenderTargets.erase(std::find(pOffScreenRenderTargets.begin(), pOffScreenRenderTargets.end(), pOffScreenRenderTarget));
	}
}