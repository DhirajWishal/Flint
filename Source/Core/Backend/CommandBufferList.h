// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Buffer.h"
#include "Pipeline.h"

namespace Flint
{
	namespace Backend
	{
		template<class TDevice, class TBuffer, class TScreenBoundRenderTarget, class TScreenBoundGraphicsPipeline, class TScreenBoundGraphicsPipelineResource>
		class CommandBufferList : public BackendObject {
		public:
			using DeviceType = TDevice;
			using BufferType = TBuffer;
			using ScreenBoundRenderTargetType = TScreenBoundRenderTarget;
			using ScreenBoundGraphicsPipelineType = TScreenBoundGraphicsPipeline;
			using ScreenBoundGraphicsPipelineResourceType = TScreenBoundGraphicsPipelineResource;

		public:
			CommandBufferList() {}
			virtual ~CommandBufferList() {}

			virtual void Initialize(DeviceType* pDevice, UI64 bufferCount) = 0;
			virtual void Terminate() = 0;

			virtual void ClearBuffers() = 0;
			virtual void ReceateBuffers() = 0;

			virtual void BeginBuffer(UI64 index) = 0;
			virtual void BeginNextBuffer() { BeginBuffer(IncrementIndex()); }
			virtual void EndBuffer() = 0;

			virtual void BindVertexBuffer(const BufferType& buffer, UI64 firstBinding, UI64 bindingCount) = 0;
			virtual void BindIndexBuffer(const BufferType& buffer) = 0;
			virtual void BindRenderTarget(const ScreenBoundRenderTargetType& renderTarget) = 0;
			virtual void EndRenderTarget() = 0;
			virtual void BindPipeline(const ScreenBoundGraphicsPipelineType& graphicsPipeline) = 0;
			virtual void SetDynamicStates(const DynamicStateContainer& container) = 0;
			virtual void BindRenderResource(const ScreenBoundGraphicsPipelineResourceType& resource) = 0;
			virtual void DrawIndexed(UI64 indexCount, UI64 indexOffset, UI64 vertexOffset) = 0;

		public:
			UI64 GetBufferCount() const { return mBufferCount; }
			UI64 GetIndex() const { return mBufferIndex; }
			void SetIndex(UI64 index) { mBufferIndex = index; }

			UI64 IncrementIndex()
			{
				mBufferIndex++;
				if (mBufferIndex >= mBufferCount) mBufferIndex = 0;
				return mBufferIndex;
			}

		protected:
			DeviceType* pDevice = nullptr;
			UI64 mBufferCount = 0;
			UI64 mBufferIndex = 0;
		};
	}
}