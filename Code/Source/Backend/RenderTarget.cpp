// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "RenderTarget.hpp"
#include "GraphicsPipeline.hpp"

namespace Flint
{
	RenderTarget::RenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList)
		: DeviceBoundObject(pDevice), mExtent(extent), mBufferCount(bufferCount), pCommandBufferList(pCommandBufferList)
	{
		if (extent.mWidth == 0 || extent.mHeight == 0)
			FLINT_THROW_INVALID_ARGUMENT("Render target width and height should be greater than 0!");

		if (bufferCount == 0)
			FLINT_THROW_INVALID_ARGUMENT("Render target buffer count should be greater than 0!");

		if (!pCommandBufferList)
			FLINT_THROW_INVALID_ARGUMENT("Render target command buffer pointer should not be null!");
	}

	void RenderTarget::SubmitPipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
	{
		INSERT_INTO_VECTOR(mDrawInstances[pGeometryStore], pPipeline);
		bIsAltered = true;
	}

	void RenderTarget::RemovePipeline(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
	{
		std::vector<std::shared_ptr<GraphicsPipeline>>& pPipelines = mDrawInstances[pGeometryStore];

		for (UI64 i = 0; i < pPipelines.size(); i++)
		{
			if (pPipelines[i] == pPipeline)
			{
				pPipelines.erase(pPipelines.begin() + i);
				break;
			}
		}

		bIsAltered = true;
	}
}