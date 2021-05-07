// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

namespace Flint
{
	namespace Interface
	{
		class Display;

		class Device : public IObject
		{
		public:
			Device() {}
			virtual ~Device() {}

			virtual bool CheckDisplayCompatibility(const Display* pDisplay) = 0;
		};
	}
}