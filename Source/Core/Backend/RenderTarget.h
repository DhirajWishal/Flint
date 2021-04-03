// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.h"
#include "CommandBufferList.h"

namespace Flint
{
	namespace Backend
	{
		template<class TDevice, class TCommandBufferList>
		class RenderTarget : public BackendObject {
		public:
			using DeviceType = TDevice;
			using CommandBufferListType = TCommandBufferList;

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
			UI64 GetBufferCount() const { return mBufferCount;  }

		protected:
			CommandBufferListType mCommandBufferList = {};

			DeviceType* pDevice = nullptr;
			Vector2 mExtent = Vector2::Zero;
			UI64 mBufferCount = 0;
		};
	}
}