// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Handles.h"
#include "IObject.h"

namespace Flint
{
	namespace Interface
	{
		class Instance : public IObject
		{
		public:
			Instance(bool enableValidation) : bEnableValidation(enableValidation) {}
			virtual ~Instance() {}

			bool IsValidationEnabled() const { return bEnableValidation; }

		protected:
			bool bEnableValidation = false;
		};
	}
}