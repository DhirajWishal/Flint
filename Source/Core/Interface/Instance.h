// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Handles.h"

namespace Flint
{
	namespace Interface
	{
		namespace Template
		{
			class Instance
			{
			public:
				Instance(bool enableValidation) : bEnableValidation(enableValidation) {}
				virtual ~Instance() {}

				virtual void Terminate() = 0;

				bool IsValidationEnabled() const { return bEnableValidation; }

			protected:
				bool bEnableValidation = false;
			};
		}

		/**
		 * Create a new instance.
		 *
		 * @param enableValidation: Whether or not to enable backend validation. If running in Release mode, we recommend not to enable this as it takes up a lot of resources.
		 * @return The created instance handle.
		 */
		inline FLINT_DEFINE_FUNCTION_POINTER(InstanceHandle, CrateInstance, bool enableValidation);

		/**
		 * Destroy a created instance.
		 *
		 * @param handle: The handle to be destroyed.
		 */
		inline FLINT_DEFINE_FUNCTION_POINTER(void, DestroyInstance, InstanceHandle handle);
	}
}