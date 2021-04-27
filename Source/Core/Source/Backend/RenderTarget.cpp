// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/RenderTarget.h"
#include "Core/ErrorHandler/Logger.h"

namespace Flint
{
	UI64 FRenderTarget::AddStaticDrawEntry(FBuffer* pVertexBuffer, FBuffer* pIndexBuffer)
	{
		mStaticDrawEntries[mStaticDrawIndex] = DrawEntry(pVertexBuffer, pIndexBuffer);
		return mStaticDrawIndex++;
	}

	void FRenderTarget::RemoveStaticDrawEntry(const UI64 index)
	{
		mStaticDrawEntries.erase(index);
	}

	UI64 FRenderTarget::AddDynamicDrawEntry(FBuffer* pVertexBuffer, FBuffer* pIndexBuffer)
	{
		mDynamicDrawEntries[mDynamicDrawIndex] = DrawEntry(pVertexBuffer, pIndexBuffer);
		return mDynamicDrawIndex++;
	}

	void FRenderTarget::RemoveDynamicDrawEntry(const UI64 index)
	{
		mDynamicDrawEntries.erase(index);
	}

	UI64 FRenderTarget::AddPipelineToStaticDrawEntry(UI64 ID, FPipeline* pPipeline)
	{
		if (mStaticDrawEntries.find(ID) != mStaticDrawEntries.end())
		{
			DrawEntry& entry = mStaticDrawEntries[ID];
			entry.pPipelines[entry.mIndex] = pPipeline;

			return entry.mIndex++;
		}
	}

	UI64 FRenderTarget::AddPipelineToDynamicDrawEntry(UI64 ID, FPipeline* pPipeline)
	{
		if (mDynamicDrawEntries.find(ID) != mDynamicDrawEntries.end())
		{
			DrawEntry& entry = mDynamicDrawEntries[ID];
			entry.pPipelines[entry.mIndex] = pPipeline;

			return entry.mIndex++;
		}
	}
}