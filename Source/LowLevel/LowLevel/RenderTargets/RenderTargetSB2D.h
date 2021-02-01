// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "LowLevel/Device.h"
#include "Core/Maths/Vector/Vector2.h"

namespace Flint
{
	namespace LowLevel
	{
		/**
		 * Screen Bound 2D Render Target object.
		 */
		class RenderTargetSB2D {
		public:
			RenderTargetSB2D() {}
			~RenderTargetSB2D() {}

			void Initialize(const Device& device, Vector2 extent, UI32 bufferCount = 0);
			void Terminate();

		public:
			Interface::RenderTargetHandleSB2D GetHandle() const { return mHandle; }

		private:
			Interface::RenderTargetHandleSB2D mHandle = FLINT_NULL_HANDLE;
		};
	}
}