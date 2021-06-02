// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.h"

namespace Flint
{
	/**
	 * Flint instance object.
	 * This object is the main object for any Flint instance.
	 */
	class FLINT_API Instance : public FObject
	{
	public:
		Instance(bool enableValidation) noexcept : mEnableValidation(enableValidation) {}

	public:
		/**
		 * Check if validation is enabled.
		 * 
		 * @return Boolean value stating true or false.
		 */
		bool IsValidationEnabled() const noexcept { return mEnableValidation; }

	protected:
		bool mEnableValidation = true;
	};

	/**
	 * Create a new instance object.
	 * 
	 * @param enableValidation: Whether or not to enable validation. We recommend using validation only in debug stage as it uses more resources.
	 * @return The instance object reference.
	 */
	FLINT_API Instance& CreateInstance(bool enableValidation);

	/**
	 * Destroy a created instance.
	 * 
	 * @param instance: The instance to destroy.
	 */
	FLINT_API void DestroyInstance(Instance& instance);
}