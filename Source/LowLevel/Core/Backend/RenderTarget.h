// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"

namespace Flint
{
	namespace Backend
	{
		class RenderTarget : public BackendObject {
		public:
			RenderTarget() {}

			virtual void Initialize(Device* pDevice) = 0;
			virtual void Terminate() = 0;

		protected:
			Device* pDevice = nullptr;
		};

		/**
		 * Software based render target.
		 */
		class RenderTargetS : public RenderTarget {
		public:
			RenderTargetS() {}
		};

		/**
		 * Hardware accelerated render target.
		 */
		class RenderTargetH : public RenderTarget {
		public:
			RenderTargetH() {}
		};
	}
}