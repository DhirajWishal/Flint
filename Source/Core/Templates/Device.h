// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

namespace Flint
{
	namespace Templates
	{
		/**
		 * Flint device template.
		 */
		class Device
		{
		public:
			Device(Instance* pInstance) {}

		protected:
			Instance* pInstance = nullptr;
		};
	}
}