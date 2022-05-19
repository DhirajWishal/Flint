// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace Flint
{
	/**
	 * FObject class.
	 * This class is the base class for all the objects in the engine.
	 */
	class FObject
	{
	public:
		/**
		 * Default constructor.
		 */
		constexpr FObject() = default;

		/**
		 * Default virtual destructor.
		 */
		virtual ~FObject() = default;

		/**
		 * Get this object casted to another type.
		 * This is intended to be used with inheritance.
		 *
		 * @tparam Type The type to cast to.
		 * @return The type pointer.
		 */
		template<class Type>
		[[nodiscard]] Type* as() { return static_cast<Type*>(this); }

		/**
		 * Get this object casted to another type.
		 * This is intended to be used with inheritance.
		 *
		 * @tparam Type The type to cast to.
		 * @return The type pointer.
		 */
		template<class Type>
		[[nodiscard]] const Type* as() const { return static_cast<const Type*>(this); }
	};
}