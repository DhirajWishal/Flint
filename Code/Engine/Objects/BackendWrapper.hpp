// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.hpp"

namespace Flint
{
	/**
	 * Flint backend wrapper object.
	 * This object is used to wrap a single backend object to be used by external applications.
	 */
	template<class Type>
	class BackendWrapper
	{
		static_assert(std::is_base_of<FObject, Type>::value, "The 'Type' must be a valid derived class of Flint::FObject!");

	public:
		BackendWrapper(Type& object) : mObject(object) {}

	public:
		/**
		 * Get the primitive backend object.
		 *
		 * @return The object reference.
		 */
		Type& GetPrimitive() { return mObject; }

		/**
		 * Get the primitive backend object.
		 *
		 * @return The object reference.
		 */
		const Type& GetPrimitive() const { return mObject; }

	public:
		/**
		 * Arrow operator overload.
		 *
		 * @return The backend object reference.
		 */
		Type& operator->() { return mObject; }

		/**
		 * Arrow operator overload.
		 *
		 * @return The backend object reference.
		 */
		const Type& operator->() const { return mObject; }

		/**
		 * Backend object reference operator
		 *
		 * @return The backend object reference.
		 */
		operator Type& () { return mObject; }

		/**
		 * Backend object reference operator
		 *
		 * @return The backend object reference.
		 */
		operator const Type& () const { return mObject; }

	protected:
		Type& mObject;
	};
}