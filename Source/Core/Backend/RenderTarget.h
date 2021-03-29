// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Display.h"

namespace Flint
{
	namespace Backend
	{
		template<class DeviceType>
		class RenderTarget : public BackendObject {
		public:
			using DeviceType = DeviceType;

		public:
			RenderTarget() {}
			virtual ~RenderTarget() {}

			virtual void Terminate() = 0;

			Vector2 GetExtent() const { return mExtent; }
			UI64 GetBufferCount() const { return mBufferCount;  }

		protected:
			DeviceType* pDevice = nullptr;
			Vector2 mExtent = Vector2::ZeroAll;
			UI64 mBufferCount = 0;
		};
	}
}