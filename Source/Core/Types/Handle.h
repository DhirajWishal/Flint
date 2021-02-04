// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DataTypes.h"
#include <type_traits>

#define FLINT_DEFINE_HANDLE(handle)				enum class handle : UI64 { INVALID = 0 }
#define FLINT_NULL_HANDLE						{}

namespace Flint
{
	/**
	 * Check if the handle type is a valid Flint Handle Type.
	 *
	 * @tparam Handle: The handle type to be checked.
	 * @return Boolean value.
	 */
	template<class Handle>
	constexpr bool IsValidHandleType() { return std::is_enum<Handle>::value && std::is_same<std::underlying_type<Handle>::type, UI64>::value; }

#define FLINT_ENFORCE_HANDLE_IS_ENUM(handle)	static_assert(::Flint::IsValidHandleType<handle>(), "Handle must be a valid Flint Handle Type! (enum class : UI64)")

	/**
	 * Check if a handle is valid.
	 *
	 * @tparam Handle: The handle type.
	 * @param handle: The handle to be checked.
	 * @return Boolean value.
	 */
	template<class Handle>
	constexpr bool IsHandleInvalid(const Handle& handle) { FLINT_ENFORCE_HANDLE_IS_ENUM(Handle); return handle == static_cast<Handle>(0); }

	/**
	 * Create a handle using an unsigned 64 bit integer.
	 *
	 * @tparam Handle: The handle type.
	 * @param integer: The integer to be casted.
	 * @return The handle.
	 */
	template<class Handle>
	constexpr Handle UI64ToHandle(const UI64& integer) { FLINT_ENFORCE_HANDLE_IS_ENUM(Handle); return static_cast<Handle>(integer); }

	/**
	 * Get a unsigned 64 bit integer from a handle.
	 *
	 * @tparam Handle: The handle type.
	 * @param handle: The handle to be casted.
	 * @return The integer value.
	 */
	template<class Handle>
	constexpr UI64 HandleToUI64(const Handle& handle) { FLINT_ENFORCE_HANDLE_IS_ENUM(Handle); return static_cast<UI64>(handle); }

	/**
	 * Create a handle from a pointer.
	 *
	 * @tparam Handle: The handle type.
	 * @param pointer: The pointer to be casted.
	 * @return The handle.
	 */
	template<class Handle>
	constexpr Handle PointerToHandle(void* pointer) { FLINT_ENFORCE_HANDLE_IS_ENUM(Handle); return UI64ToHandle<Handle>(reinterpret_cast<UI64>(pointer)); }

	/**
	 * Get a pointer from the handle.
	 *
	 * @tparam Handle: The handle type.
	 * @param handle: The handle to be casted.
	 * @return The pointer.
	 */
	template<class Handle>
	constexpr void* HandleToPointer(const Handle& handle) { FLINT_ENFORCE_HANDLE_IS_ENUM(Handle); return reinterpret_cast<void*>(handle); }
}
