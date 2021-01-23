// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/Handle.h"
#include "Core/Inputs/InputCenter.h"

#include "Interface/Device.h"

namespace Flint
{
	/**
	 * Flint Backend Interface.
	 * The interface is made up of function pointers which will be loaded at runtime after selecting the required
	 * backend. This way it removes the additional lookups which happens in inheritance this improving performance.
	 * This also allows the objects to be static allocated and removes the additional heap allocations.
	 *
	 * All of these interface functions MUST BE CREATED in the backend and must be exported when selected.
	 *
	 * Additionally the backend resources are managed by the backend itself.
	 */
	namespace Interface
	{
		/**
		 * Check if the interface functions are initialized.
		 *
		 * @return Boolean value.
		 */
		bool IsInitialized();
	}
}