// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/RenderTarget.h"
#include "Core/ErrorHandler/Logger.h"

namespace Flint
{
	namespace Backend
	{
		namespace _Helpers
		{
			void BakeCommands(const std::vector<RenderTarget::DrawEntry>& mEntries, const std::shared_ptr<CommandBuffer>& pCommandBuffer)
			{

			}

			void RuntimeCommandBaker(std::shared_ptr<CommandBufferManager> pCommandBufferManager, Thread::SharedDataClient<RenderTarget::ThreadPayload>* pClient)
			{
			}
		}

		EntryReference RenderTarget::AddDrawEntry(const WireFrame& wireFrame, Pipeline* pPipeline, PipelineResource* pPipelineResource, const DynamicStateContainer& container)
		{
			DrawEntry entry = {};
			entry.mWireFrame = wireFrame;
			entry.pPipeline = pPipeline;
			entry.pResource = pPipelineResource;
			entry.mDynamicStates = container;
			mDynamicDrawEntries[mReferenceCounter++] = std::move(entry);

			return mReferenceCounter - 1;
		}

		EntryReference RenderTarget::AddStaticDrawEntry(const WireFrame& wireFrame, Pipeline* pPipeline, PipelineResource* pPipelineResource, const DynamicStateContainer& container)
		{
			DrawEntry entry = {};
			entry.mWireFrame = wireFrame;
			entry.pPipeline = pPipeline;
			entry.pResource = pPipelineResource;
			entry.mDynamicStates = container;
			mStaticDrawEntries[mReferenceCounter++] = std::move(entry);

			return mReferenceCounter - 1;
		}

		void RenderTarget::RemoveDrawEntry(EntryReference reference)
		{
			// TODO
		}

		void RenderTarget::PrepareCommandBuffers()
		{
			if (mDynamicDrawEntries.Size())
			{
				INSERT_INTO_VECTOR(mChildCommandBuffers, pCommandBufferManager->CreateChild(1));
				return;
			}

			auto& commandBuffers = pCommandBufferManager->GetBuffers();

			for (auto itr = commandBuffers.begin(); itr != commandBuffers.end(); itr++)
			{
				(*itr)->BeginBufferRecording();
				Bind(*itr);

				for (auto& pEntry = mStaticDrawEntries.Begin(); pEntry != mStaticDrawEntries.End(); pEntry++)
				{
					pEntry->second.pPipeline->Bind(*itr);
					pEntry->second.mWireFrame.pVertexBuffer->Bind(*itr);
					pEntry->second.mWireFrame.pIndexBuffer->Bind(*itr);

					for (auto i = pEntry->second.mWireFrame.mDrawData.begin(); i != pEntry->second.mWireFrame.mDrawData.end(); i++)
					{
						if (pEntry->second.pResource)
							pEntry->second.pResource->Bind(*itr);

						(*itr)->SubmitDynamicStates(pEntry->second.mDynamicStates);
						(*itr)->DrawUsingIndexData(static_cast<UI32>(i->mIndexCount), static_cast<UI32>(i->mVertexOffset), static_cast<UI32>(i->mIndexOffset));
					}
				}

				UnBind(*itr);
				(*itr)->EndBufferRecording();

				mFrameIndex++;
			}
		}

		void RenderTarget::BakeDynamicCommands(const std::vector<RenderTarget::DrawEntry>& mEntries, const std::shared_ptr<CommandBufferManager>& pCommandBufferManager)
		{
			auto& pBuffers = pCommandBufferManager->GetBuffers();
			const UI64 shift = mEntries.size() / pBuffers.size();

			for (UI64 i = 0; i < pBuffers.size(); i++)
			{
				auto& pCommandBuffer = pBuffers[i];
				pCommandBuffer->BeginBufferRecording();

				for (UI64 j = shift * i; j < shift * (i + 1); j++)
				{
					auto& entry = mEntries[j];

					entry.pPipeline->Bind(pCommandBuffer);
					entry.mWireFrame.pVertexBuffer->Bind(pCommandBuffer);
					entry.mWireFrame.pIndexBuffer->Bind(pCommandBuffer);

					for (auto itr = entry.mWireFrame.mDrawData.begin(); itr != entry.mWireFrame.mDrawData.end(); itr++)
					{
						if (entry.pResource)
							entry.pResource->Bind(pCommandBuffer);

						pCommandBuffer->SubmitDynamicStates(entry.mDynamicStates);
						pCommandBuffer->DrawUsingIndexData(static_cast<UI32>(itr->mIndexCount), static_cast<UI32>(itr->mVertexOffset), static_cast<UI32>(itr->mIndexOffset));
					}
				}

				pCommandBuffer->EndBufferRecording();
			}
		}
		
		void RenderTarget::DestroyChildCommandBuffers()
		{
			for (auto ptr : mChildCommandBuffers)
				ptr->Terminate();

			mChildCommandBuffers.clear();
		}
	}
}