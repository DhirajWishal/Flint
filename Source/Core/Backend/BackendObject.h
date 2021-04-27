// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <type_traits>
#include "Core\Macros\Global.h"

namespace Flint
{
	/**
	 * Backend object.
	 * This object is the base class for all the backend objects in the library.
	 */
	class BackendObject {
	public:
		BackendObject() {}
		virtual ~BackendObject() {}

		/**
		 * Cast the object to a derived type.
		 *
		 * @tparam Derived: The derived type to cast to.
		 * @return The casted pointer.
		 */
		template<class Derived>
		Derived* Derive() { return dynamic_cast<Derived*>(this); }

		/**
		 * Cast the object to a derived type.
		 *
		 * @tparam Derived: The derived type to cast to.
		 * @return The casted pointer.
		 */
		template<class Derived>
		const Derived* Derive() const { return dynamic_cast<Derived*>(this); }

		/**
		 * Cast this object to another type.
		 *
		 * @tparam Type: The type to be casted into.
		 * @return The casted pointer.
		 */
		template<class Type>
		Type* GetAs() { return static_cast<Type*>(this); }

		/**
		 * Cast this object to another type.
		 *
		 * @tparam Type: The type to be casted into.
		 * @return The casted pointer.
		 */
		template<class Type>
		const Type* GetAs() const { return static_cast<Type*>(this); }
	};
}