// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Display.h"

namespace Flint
{
	namespace Backend
	{
		template<class Derived, class DeviceType>
		class RenderTarget : public BackendObject<Derived> {
		public:
			using DeviceType = DeviceType;

		public:
			RenderTarget() {}
			virtual ~RenderTarget() {}

			void Terminate() { GetDerived().mTerminate(); }

			Vector2 GetExtent() const { return mExtent; }
			UI64 GetBufferCount() const { return mBufferCount;  }

		protected:
			virtual void mInitialize() = 0;
			virtual void mTerminate() = 0;

			std::shared_ptr<DeviceType> pDevice = {};
			Vector2 mExtent = Vector2::ZeroAll;
			UI64 mBufferCount = 0;
		};
	}
}