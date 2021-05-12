// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/RenderTarget.h"
#include "Core/ErrorHandler/Logger.h"

namespace Flint
{
	namespace Backend
	{
		UI64 RenderTarget::AddStaticDrawEntry(Buffer* pVertexBuffer, Buffer* pIndexBuffer)
		{
			mStaticDrawEntries[mStaticDrawIndex] = DrawEntry(pVertexBuffer, pIndexBuffer);
			return mStaticDrawIndex++;
		}

		void RenderTarget::RemoveStaticDrawEntry(const UI64 index)
		{
			mStaticDrawEntries.erase(index);
		}

		UI64 RenderTarget::AddDynamicDrawEntry(Buffer* pVertexBuffer, Buffer* pIndexBuffer)
		{
			mDynamicDrawEntries[mDynamicDrawIndex] = DrawEntry(pVertexBuffer, pIndexBuffer);
			return mDynamicDrawIndex++;
		}

		void RenderTarget::RemoveDynamicDrawEntry(const UI64 index)
		{
			mDynamicDrawEntries.erase(index);
		}

		UI64 RenderTarget::AddPipelineToStaticDrawEntry(UI64 ID, FPipeline* pPipeline)
		{
			if (mStaticDrawEntries.find(ID) != mStaticDrawEntries.end())
			{
				DrawEntry& entry = mStaticDrawEntries[ID];
				entry.pPipelines[entry.mIndex] = pPipeline;

				return entry.mIndex++;
			}

			return 0;
		}

		UI64 RenderTarget::AddPipelineToDynamicDrawEntry(UI64 ID, FPipeline* pPipeline)
		{
			if (mDynamicDrawEntries.find(ID) != mDynamicDrawEntries.end())
			{
				DrawEntry& entry = mDynamicDrawEntries[ID];
				entry.pPipelines[entry.mIndex] = pPipeline;

				return entry.mIndex++;
			}

			return 0;
		}
	}
}