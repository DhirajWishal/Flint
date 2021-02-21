// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendObject.h"

namespace Flint
{
	namespace Backend
	{
		class CommandBufferManager;

		/**
		 * Command buffer object.
		 * This object is used to submit draw and other utility commands to the GPU device.
		 */
		class CommandBuffer : public BackendObject {
			friend CommandBufferManager;

		public:
			CommandBuffer() {}
			CommandBuffer(CommandBufferManager* parent, UI64 index) : pParent(parent), mIndex(index), bIsValid(true) {}

			CommandBufferManager* GetManager() const { return pParent; }
			UI64 GetIndex() const { return mIndex; }
			bool IsValid() const { return bIsValid; }

		private:
			CommandBufferManager* pParent = nullptr;
			UI64 mIndex = 0;

			bool bIsValid = false;
		};
	}
}