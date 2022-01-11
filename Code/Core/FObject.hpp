// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"
#include <type_traits>

namespace Flint
{
	class FObject;

	/**
	 * Concept which only accepts object types which are derived from the FObject class.
	 */
	template<class Type>
	concept DerivedFromFObject = std::is_base_of_v<Type, FObject>;

	/**
	 * Flint object class.
	 * This is the base class for all the Flint objects and contains common and utility functions.
	 */
	class FObject
	{
	public:
		FObject() = default;
		virtual ~FObject() = default;

	public:
		/**
		 * Dynamic cast the object to a derived class.
		 *
		 * @tparam Derived: The derived type.
		 * @return The derived class reference.
		 */
		template<DerivedFromFObject Derived>
		Derived& DynamicCast() { return *dynamic_cast<Derived*>(this); }

		/**
		 * Static cast the object to another type.
		 * If used for dynamic casting, this performs a touch better than DynamicCast.
		 *
		 * @tparam Type: The type to cast to.
		 * @return The casted type reference.
		 */
		template<class Type>
		Type& StaticCast() { return *static_cast<Type*>(this); }

		/**
		 * Static cast the object to another type.
		 * If used for dynamic casting, this performs a touch better than DynamicCast.
		 *
		 * @tparam Type: The type to cast to.
		 * @return The casted type reference.
		 */
		template<class Type>
		const Type& StaticCast() const { return *static_cast<const Type*>(this); }

	protected:
		bool bIsTerminated = false;
	};
}