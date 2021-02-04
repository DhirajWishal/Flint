// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendObject.h"

namespace Flint
{
	namespace Backend
	{
		class Instance : public BackendObject {
		public:
			Instance() {}

			virtual void Initialize(bool enableValidation) = 0;
			virtual void Terminate() = 0;

		protected:
			bool mEnableValidation = true;
		};
	}
}