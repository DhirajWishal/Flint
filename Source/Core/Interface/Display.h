// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
#include "Core\Maths\Vector\Vector2.h"
#include "Core\Inputs\InputCenter.h"

namespace Flint
{
	namespace Interface
	{
		class Display
		{
		public:
			Display(Inputs::InputCenter* pInputCenter) : pInputCenter(pInputCenter) {}
			virtual ~Display() {}

			Inputs::InputCenter* GetInputCenter() { return pInputCenter; }
			const Inputs::InputCenter* GetInputCenter() const { return pInputCenter; }

		protected:
			Inputs::InputCenter* pInputCenter = nullptr;
		};
	}
}