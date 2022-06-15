// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#define FLINT_TERMINATE_IF_VALID	if(m_IsValid) terminate()

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
		 * Terminate function.
		 * This function is used to terminate object.
		 */
		virtual void terminate() = 0;

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

		/**
		 * Check if the object is valid or not.
		 *
		 * @return The status.
		 */
		[[nodiscard]] bool isValid() const { return m_IsValid; }

	protected:
		/**
		 * Validate the object.
		 * This should be done once an object is valid.
		 */
		void validate() { m_IsValid = true; }

		/**
		 * Invalidate the object.
		 * This must be done by the terminate function.
		 */
		void invalidate() { m_IsValid = false; }

	protected:
		bool m_IsValid = false;
	};
}