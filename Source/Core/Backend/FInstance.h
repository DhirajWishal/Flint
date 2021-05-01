// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendObject.h"

#include "Core\Types\DataTypes.h"
#include "Core\Macros\Global.h"
#include "Core\Maths\Vector\Vector2.h"

#include "Core\Types\Handle.h"

#include <memory>

#define FLINT_BACKEND_OBJECT_ASSERT(base, derived, msg) static_assert(!std::is_base_of<base, derived>::value, msg);

namespace Flint
{
	class FDevice;
	class FDisplay;

	/**
	 * Flint instance object.
	 * This object is the basis of all the backend objects. This represents a single backend API instance.
	 */
	class FInstance : public BackendObject, public std::enable_shared_from_this<FInstance> {
	public:
		FInstance(bool enableValidation) {}
		virtual ~FInstance() {}

		/**
		 * Check if validation is enabled.
		 *
		 * @return The boolean stating if the validation is enabled.
		 */
		bool IsValidationEnabled() const { return bEnableValidation; }

	public:
		/**
		 * Create a new device object.
		 * 
		 * @return the newly created device object pointer.
		 */
		virtual std::shared_ptr<FDevice> CreateDevice() = 0;

		/**
		 * Create a new display object.
		 * 
		 * @param extent: The extent of the display.
		 * @param pTitle: The title of the display.
		 * @return The newly created display object pointer.
		 */
		virtual std::shared_ptr<FDisplay> CreateDisplay(const Vector2 extent, const char* pTitle) = 0;

	protected:
		bool bEnableValidation = true;
	};

	namespace Interface
	{
		/**
		 * The instance handle.
		 */
		FLINT_DEFINE_HANDLE(InstanceHandle);

		/**
		 * Create a new instance handle.
		 * 
		 * @param enableValidation: Whether or not to enable backend validation.
		 * @return The instance handle.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(InstanceHandle, CreateInstance, bool enableValidation);

		/**
		 * Destroy a created instance.
		 * 
		 * @param instance: The instance to be destroyed.
		 */
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyInstance, InstanceHandle instance);
	}
}