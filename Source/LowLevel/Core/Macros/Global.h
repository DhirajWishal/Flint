// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#define FLINT_INLINE						inline
#define FLINT_FORCEINLINE					__forceinline

#define BIT_SHIFT(count)					(1 << count)

#define TYPE_NAME(type)						typeid(type).name()
