// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendObject.h"

namespace Flint
{
	namespace Backend
	{
		class Instance : public BackendObject
		{
		public:
			Instance(bool enableValidation) : bEnableValidation(enableValidation) {}
			virtual ~Instance() {}

			bool IsValidationEnabled() const { return bEnableValidation; }

		protected:
			bool bEnableValidation = true;
		};
	}
}