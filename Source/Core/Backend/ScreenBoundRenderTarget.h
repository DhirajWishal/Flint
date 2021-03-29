// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.h"

namespace Flint
{
	namespace Backend
	{
		template<class Derived, class DeviceType, class DisplayType>
		class ScreenBoundRenderTarget : public RenderTarget<Derived, DeviceType> {
		public:
			using DisplayType = DisplayType;

		public:
			ScreenBoundRenderTarget() {}
			virtual ~ScreenBoundRenderTarget() {}

			void Initialize(std::shared_ptr<DeviceType> pDevice, std::shared_ptr<DisplayType> pDisplay, UI64 bufferCount)
			{
				this->pDevice = pDevice, this->pDisplay = pDisplay, this->mBufferCount = bufferCount, this->mExtent = pDisplay->GetExtent();
				GetDerived().mInitialize();
			}

			UI32 GetFrameIndex() const { return mFrameIndex; }
			UI32 GetImageIndex() const { return mImageIndex; }

		protected:
			void IncrementIndex() 
			{
				mFrameIndex++;
				if (mFrameIndex >= mBufferCount) mFrameIndex = 0;
			}

			std::shared_ptr<DeviceType> pDevice = {};
			std::shared_ptr<DisplayType> pDisplay = {};

			UI32 mFrameIndex = 0;
			UI32 mImageIndex = 0;
		};
	}
}