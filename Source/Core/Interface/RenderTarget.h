// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Display.h"

namespace Flint
{
	namespace Interface
	{
		class RenderTarget : public IObject
		{
		public:
			RenderTarget(Device* pDevice, Vector2 extent, UI64 bufferCount) : pDevice(pDevice), mExtent(extent), mBufferCount(bufferCount) {}
			virtual ~RenderTarget() {}

		protected:
			Device* pDevice = nullptr;
			Vector2 mExtent = Vector2::Zero;
			UI64 mBufferCount = 0;
		};
	}
}