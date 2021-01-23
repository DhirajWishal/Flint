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
		inline InstanceHandle(*CreateInstance)(bool enableValidation) = nullptr;

		/**
		 * Destroy a created backend instance.
		 *
		 * @param instance: The instance handle.
		 */
		inline void (*DestroyInstance)(const InstanceHandle& instance) = nullptr;
	}
}