// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Macros.h"
#include <cstdint>
#include <limits>

typedef uint8_t UI8;
typedef uint16_t UI16;
typedef uint32_t UI32;
typedef uint64_t UI64;

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef wchar_t wchar;

typedef UI8 BYTE;
typedef BYTE* BPTR;

#define INSERT_INTO_VECTOR(vector, ...)		vector.insert(vector.end(), __VA_ARGS__)

#define UI8_MIN								std::numeric_limits<UI8>::min()
#define UI16_MIN							std::numeric_limits<UI16>::min()
#define UI32_MIN							std::numeric_limits<UI32>::min()
#define UI64_MIN							std::numeric_limits<UI64>::min()

#define UI8_MAX								std::numeric_limits<UI8>::max()
#define UI16_MAX							std::numeric_limits<UI16>::max()
#define UI32_MAX							std::numeric_limits<UI32>::max()
#define UI64_MAX							std::numeric_limits<UI64>::max()

#define I8_MIN								std::numeric_limits<I8>::min()
#define I16_MIN								std::numeric_limits<I16>::min()
#define I32_MIN								std::numeric_limits<I32>::min()
#define I64_MIN								std::numeric_limits<I64>::min()

#define I8_MAX								std::numeric_limits<I8>::max()
#define I16_MAX								std::numeric_limits<I16>::max()
#define I32_MAX								std::numeric_limits<I32>::max()
#define I64_MAX								std::numeric_limits<I64>::max()
