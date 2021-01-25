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
		 * Screen Bound 3D Render Target object.
		 */
		class RenderTargetSB3D {
		public:
			RenderTargetSB3D() {}
			~RenderTargetSB3D() {}

			void Initialize(const Device& device, Vector2 extent, UI32 bufferCount = 0);
			void Terminate();

		private:
			Interface::RenderTargetHandleSB3D mHandle = {};
		};
	}
}