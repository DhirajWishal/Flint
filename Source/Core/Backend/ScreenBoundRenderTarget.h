// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.h"
#include "Pipeline.h"

namespace Flint
{
	namespace Backend
	{
		template<class TDevice, class TDisplay, class TCommandBufferList, class TGraphicsPipeline>
		class ScreenBoundRenderTarget : public RenderTarget<TDevice, TCommandBufferList> {
		public:
			using DeviceType = TDevice;
			using DisplayType = TDisplay;

		public:
			ScreenBoundRenderTarget() {}
			virtual ~ScreenBoundRenderTarget() {}

			virtual void Initialize(DeviceType* pDevice, DisplayType* pDisplay, UI64 bufferCount) = 0;

			void RegisterGraphicsPipelineStatic(TGraphicsPipeline* pPipeline) { INSERT_INTO_VECTOR(pGraphcisPipelinesStatic, pPipeline); }
			void RegisterGraphicsPipelineDynamic(TGraphicsPipeline* pPipeline) { INSERT_INTO_VECTOR(pGraphcisPipelinesDynamic, pPipeline); }

			UI32 GetFrameIndex() const { return mFrameIndex; }
			UI32 GetImageIndex() const { return mImageIndex; }

		protected:
			void IncrementIndex() { mFrameIndex++; if (mFrameIndex >= mBufferCount) mFrameIndex = 0; }

			std::vector<TGraphicsPipeline*> pGraphcisPipelinesStatic;
			std::vector<TGraphicsPipeline*> pGraphcisPipelinesDynamic;

			DisplayType* pDisplay = nullptr;

			UI32 mFrameIndex = 0;
			UI32 mImageIndex = 0;
		};
	}
}