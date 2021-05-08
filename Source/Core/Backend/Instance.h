// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendObject.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Instance object.
		 * This object holds information about a single backend instance.
		 */
		class Instance : public BackendObject
		{
		public:
			Instance(bool enableValidation) : bEnableValidation(enableValidation) {}
			virtual ~Instance() {}

			/**
			 * Check if validation is enabled.
			 */
			bool IsValidationEnabled() const { return bEnableValidation; }

		protected:
			bool bEnableValidation = true;
		};
	}
}