// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/FRenderTargetManager.h"

namespace Flint
{
	namespace Templates
	{
		class RenderTarget
		{
		public:
			RenderTarget(const FExtent2D& extent, UI32 bufferCount) : mExtent(extent), mBufferCount(bufferCount) {}

			FExtent2D GetExtent() const { return mExtent; }
			UI32 GetBufferCount() const { return mBufferCount; }
			UI32 GetCurrentFrameIndex() const { return mCurrentFrameIndex; }
			UI32 GetCurrentImageIndex() const { return mCurrentImageIndex; }

		protected:
			FExtent2D mExtent = {};
			UI32 mBufferCount = 0;

			UI32 mCurrentFrameIndex = 0;
			UI32 mCurrentImageIndex = 0;
		};
	}
}