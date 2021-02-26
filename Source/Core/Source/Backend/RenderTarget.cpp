// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/RenderTarget.h"
#include "Core/ErrorHandler/Logger.h"
#include "Core/Backend/Pipeline.h"

namespace Flint
{
	namespace Backend
	{
		EntryReference RenderTarget::AddDrawEntry(const WireFrame& wireFrame, Pipeline* pPipeline, PipelineResource* pPipelineResource)
		{
			DrawEntry entry = {};
			entry.mWireFrame = wireFrame;
			entry.pPipeline = pPipeline;
			entry.pResource = pPipelineResource;
			mDrawEntries[mReferenceCounter++] = std::move(entry);

			return mReferenceCounter - 1;
		}

		void RenderTarget::RemoveDrawEntry(EntryReference reference)
		{
			// TODO
		}

		void RenderTarget::PrepareCommandBuffers()
		{
			auto& commandBuffers = pCommandBufferManager->GetBuffers();

			for (auto itr = commandBuffers.begin(); itr != commandBuffers.end(); itr++)
			{
				pCommandBufferManager->BeginBufferRecording(static_cast<UI32>(itr->GetIndex()));
				Bind(*itr);

				for (EntryReference index = 0; index < mReferenceCounter; index++)
				{
					auto& entry = mDrawEntries[index];

					entry.pPipeline->Bind(*itr);
					entry.mWireFrame.pVertexBuffer->Bind(*itr);
					entry.mWireFrame.pIndexBuffer->Bind(*itr);

					for (auto i = entry.mWireFrame.mDrawData.begin(); i != entry.mWireFrame.mDrawData.end(); i++)
					{
						if (entry.pResource)
							entry.pResource->Bind(*itr);

						pCommandBufferManager->DrawUsingIndexData(static_cast<UI32>(itr->GetIndex()), static_cast<UI32>(i->mIndexCount), static_cast<UI32>(i->mVertexOffset), static_cast<UI32>(i->mIndexOffset));
					}
				}

				UnBind(*itr);
				pCommandBufferManager->EndBufferRecording(static_cast<UI32>(itr->GetIndex()));
			}
		}

		CommandBuffer ScreenBoundRenderTarget::GetCommandBuffer()
		{
			mFrameIndex++;
			if (mFrameIndex >= mBufferCount) mFrameIndex = 0;
			mImageIndex = pCommandBufferManager->BeginCommandExecution(mFrameIndex, this);

			if (mImageIndex < 0)
			{
				FLINT_LOG_ERROR(TEXT("Failed to acquire image index! (returned index: #2)"), mImageIndex)
					return {};
			}

			return pCommandBufferManager->GetBuffers()[mFrameIndex];
		}

		void ScreenBoundRenderTarget::Draw(const CommandBuffer& commandBuffer)
		{
			pCommandBufferManager->SubmitCommand(static_cast<UI32>(commandBuffer.GetIndex()), this);
		}
	}
}