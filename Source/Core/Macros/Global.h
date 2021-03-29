// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#define FLINT_INLINE						inline
#define FLINT_FORCEINLINE					__forceinline

#define BIT_SHIFT(count)					(1 << count)

#define TYPE_NAME(type)						typeid(type).name()
#define CREATE_COLOR_256(color)				(color / 255.0f)

#define FLINT_DEFINE_HANDLE(name)						enum class name : UI64 { INVALID = 0 }
#define FLINT_DEFINE_FUNCTION_POINTER(ret, name, ...)	inline ret (*name)(__VA_ARGS__) = nullptr

#define FLINT_SET_NO_COPY(name)		name(const name&) = delete; name& operator=(const name&) = delete;
#define FLINT_SET_NO_MOVE(name)		name(name&&) = delete; name& operator=(name&&) = delete;
#define FLINT_SET_NO_COPY_AND_MOVE(name)	FLINT_SET_NO_COPY(name) FLINT_SET_NO_MOVE(name)
