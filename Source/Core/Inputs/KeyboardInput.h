// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ButtonInput.h"

namespace Flint
{
	namespace Inputs
	{
		/**
		 * Key Input obect.
		 */
		class KeyInput : public ButtonInput {
		public:
			KeyInput() {}
			~KeyInput() {}
		};
	}
}