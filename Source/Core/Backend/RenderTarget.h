// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.h"
#include "CommandBufferList.h"

namespace Flint
{
	namespace Backend
	{
		template<class TDevice, class TBuffer, class TPipeline, class TCommandBufferList>
		class RenderTarget : public BackendObject {
		public:
			using DeviceType = TDevice;
			using CommandBufferListType = TCommandBufferList;

			struct DrawEntry {
				DrawEntry() = default;
				DrawEntry(TBuffer* pVertexBuffer, TBuffer* pIndexBuffer) : pVertexBuffer(pVertexBuffer), pIndexBuffer(pIndexBuffer) {}

				std::unordered_map<UI64, TPipeline*> pPipelines = {};

				TBuffer* pVertexBuffer = nullptr;
				TBuffer* pIndexBuffer = nullptr;

				UI64 mIndex = 0;
			};

		public:
			RenderTarget() {}
			virtual ~RenderTarget() {}

			virtual void Terminate() = 0;

			virtual void BakeCommands() = 0;
			virtual void PrepareToDraw() = 0;
			virtual void Update() = 0;
			virtual void SubmitCommand() = 0;

			DeviceType* GetDevice() const { return pDevice; }
			Vector2 GetExtent() const { return mExtent; }
			UI64 GetBufferCount() const { return mBufferCount; }

		public:
			UI64 AddStaticDrawEntry(TBuffer* pVertexBuffer, TBuffer* pIndexBuffer)
			{
				mStaticDrawEntries[mStaticDrawIndex] = DrawEntry(pVertexBuffer, pIndexBuffer);
				return mStaticDrawIndex++;
			}

			UI64 AddDynamicDrawEntry(TBuffer* pVertexBuffer, TBuffer* pIndexBuffer)
			{
				mDynamicDrawEntries[mDynamicDrawIndex] = DrawEntry(pVertexBuffer, pIndexBuffer);
				return mDynamicDrawIndex++;
			}

			UI64 AddPipelineToStaticDrawEntry(UI64 ID, TPipeline* pPipeline)
			{
				if (mStaticDrawEntries.find(ID) != mStaticDrawEntries.end())
				{
					DrawEntry& entry = mStaticDrawEntries[ID];
					entry.pPipelines[entry.mIndex] = pPipeline;

					return entry.mIndex++;
				}
			}

			UI64 AddPipelineToDynamicDrawEntry(UI64 ID, TPipeline* pPipeline)
			{
				if (mDynamicDrawEntries.find(ID) != mDynamicDrawEntries.end())
				{
					DrawEntry& entry = mDynamicDrawEntries[ID];
					entry.pPipelines[entry.mIndex] = pPipeline;

					return entry.mIndex++;
				}
			}

			std::unordered_map<UI64, DrawEntry>& GetStaticDrawEntries() { return mStaticDrawEntries; }
			const std::unordered_map<UI64, DrawEntry> GetStaticDrawEntries() const { return mStaticDrawEntries; }

			std::unordered_map<UI64, DrawEntry>& GetDynamicDrawEntries() { return mDynamicDrawEntries; }
			const std::unordered_map<UI64, DrawEntry> GetDynamicDrawEntries() const { return mDynamicDrawEntries; }

		protected:
			std::unordered_map<UI64, DrawEntry> mStaticDrawEntries;
			std::unordered_map<UI64, DrawEntry> mDynamicDrawEntries;

			CommandBufferListType mCommandBufferList = {};

			UI64 mStaticDrawIndex = 0;
			UI64 mDynamicDrawIndex = 0;

			DeviceType* pDevice = nullptr;
			Vector2 mExtent = Vector2::Zero;
			UI64 mBufferCount = 0;
		};
	}
}