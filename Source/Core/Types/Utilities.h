// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Macros/Global.h"
#include "Core/ErrorHandler/Logger.h"

namespace Flint
{
	/**
	 * Cast (C++ - Style) data type of one variable to another.
	 *
	 * @param value: Variable value.
	 * @tparam CastTo: To cast to.
	 * @tparam CastFrom: To cast from.
	 */
	template<class CastTo, class CastFrom>
	FLINT_FORCEINLINE constexpr CastTo Cast(const CastFrom& value)
	{
		return static_cast<CastTo>(value);
	}

	/**
	 * Raw cast (C - Style) data type of one variable to another.
	 *
	 * @param value: Variable value.
	 * @tparam CastTo: To cast to.
	 * @tparam CastFrom: To cast from.
	 */
	template<class CastTo, class CastFrom>
	FLINT_FORCEINLINE constexpr CastTo RawCast(const CastFrom& value)
	{
		return (CastTo)value;
	}

	/**
	 * Cast to the required derived type and dereference it.
	 *
	 * @param pBase: Base class pointer.
	 * @tparam Derived: Class to be derived to.
	 * @tparam Base: Base class of the deriving class.
	 */
	template<class Derived, class Base>
	FLINT_FORCEINLINE constexpr Derived InheritCast(Base* pBase)
	{
		return dynamic_cast<Derivev*>(pBase);
	}

	/**
	 * Cast to the required derived type.
	 *
	 * @param pBase: Base class pointer.
	 * @tparam Derived: Class to be derived to.
	 * @tparam Base: Base class of the deriving class.
	 */
	template<class Derived, class Base>
	FLINT_FORCEINLINE constexpr Derived* Inherit(Base* pBase)
	{
		return dynamic_cast<Derived*>(pBase);
	}

	/**
	 * Get the pointer as an unsigned integer.
	 *
	 * @param pointer: Pointer.
	 */
	FLINT_FORCEINLINE constexpr UI64 GetPointerAsInteger(const void* pointer)
	{
		return (UI64)pointer;
	}

	/**
	 * Add a byte offset to a given address.
	 * This function does not alter the argument address or the byte size.
	 *
	 * @param baseAddress: Base address to be offset.
	 * @param byteCount: Offset.
	 */
	FLINT_FORCEINLINE void* IncrementPointer(const void* pointer, UI64 byteCount)
	{
		return (void*)(((UI64)pointer) + byteCount);
	}

	/**
	 * Subtract a byte offset to a given address.
	 * This function does not alter the argument address or the byte size.
	 *
	 * @param baseAddress: Base address to be offset.
	 * @param byteCount: Offset.
	 */
	FLINT_FORCEINLINE void* DecrementPointer(const void* pointer, UI64 byteCount)
	{
		return (void*)(((UI64)pointer) - byteCount);
	}

	/**
	 * Check if a class is inherited from another class.
	 * This function determines it at compile time.
	 *
	 * @tparam Base: The base class to be checked from.
	 * @tparam Derived: The class to test from.
	 */
	template<class Base, class Derived>
	FLINT_FORCEINLINE constexpr bool IsInheritedFrom_C(const Derived& v)
	{
		return std::is_base_of<Base, Derived>::value;
	}

	/**
	 * Check if a class is inherited from another class.
	 * This function determines it at runtime.
	 *
	 * @tparam Base: The base class to be checked from.
	 * @tparam Derived: The class to test from.
	 */
	template<class Base, class Derived>
	FLINT_FORCEINLINE bool IsInheritedFrom_R(const Derived& v)
	{
		return dynamic_cast<Base*>(&v) != nullptr;
	}

	/**
	 * Convert wchar string to a char string.
	 *
	 * @param string: The const wchar* string.
	 * @return std::string object.
	 */
	std::string WStringToString(const std::wstring& string);

	/**
	 * Convert char string to a wchar string.
	 *
	 * @param string: The const char* string.
	 * @return std::wstring object.
	 */
	std::wstring StringToWString(const std::string& string);
}
