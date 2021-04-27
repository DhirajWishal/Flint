// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendObject.h"
#include <memory>

#include "Core\Types\DataTypes.h"
#include "Core\Macros\Global.h"

#define FLINT_BACKEND_OBJECT_ASSERT(base, derived, msg) static_assert(!std::is_base_of<base, derived>::value, msg);

namespace Flint
{
	/**
	 * Flint instance object.
	 * This object is the basis of all the backend objects. This represents a single backend API instance.
	 */
	class FInstance : public BackendObject {
	public:
		FInstance() {}
		virtual ~FInstance() {}

		/**
		 * Initialize the instance.
		 *
		 * @param enableValidation: Whether or not to enable backend validation.
		 */
		virtual void Initialize(bool enableValidation) = 0;

		/**
		 * Terminate the instance.
		 */
		virtual void Terminate() = 0;

		/**
		 * Check if validation is enabled.
		 *
		 * @return The boolean stating if the validation is enabled.
		 */
		bool IsValidationEnabled() const { return bEnableValidation; }

	protected:
		bool bEnableValidation = true;
	};
}