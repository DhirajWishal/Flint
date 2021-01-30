// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Handles.h"

namespace Flint
{
	namespace Interface
	{
		/**
		 * Create a new backend instance.
		 *
		 * @param enableValidation: Enable backend validation.
		 * @return The instance handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(InstanceHandle, CreateInstance, bool enableValidation);

		/**
		 * Destroy a created backend instance.
		 *
		 * @param instance: The instance handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyInstance, const InstanceHandle& instance);
	}
}