// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

typedef uint8_t UI8;			// Unsigned 8 bit integer.
typedef uint16_t UI16;			// Unsigned 16 bit integer.
typedef uint32_t UI32;			// Unsigned 32 bit integer.
typedef uint64_t UI64;			// Unsigned 64 bit integer.

typedef int8_t I8;				// 8 bit integer.
typedef int16_t I16;			// 16 bit integer.
typedef int32_t I32;			// 32 bit integer.
typedef int64_t I64;			// 64 bit integer.

#define TEXT(text)  L##text
typedef std::string String;
typedef std::wstring WString;
typedef wchar_t wchar;

typedef UI8 BYTE;				// Byte of data.
typedef BYTE* BPTR;				// Byte pointer.

#define INSERT_INTO_VECTOR(vector, data)	vector.insert(vector.end(), data)
