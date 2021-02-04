// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.h"

namespace Flint
{
	namespace Backend
	{
		class Device : public BackendObject {
		public:
			Device() {}

			virtual void Initialize(Display* pDisplay) = 0;
			virtual void Terminate() = 0;

		public:
			Display* GetDisplay() const { return pDisplay; }

		protected:
			Display* pDisplay = nullptr;
		};
	}
}