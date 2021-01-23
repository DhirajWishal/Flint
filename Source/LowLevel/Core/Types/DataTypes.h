// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

typedef unsigned char UI8;			// Unsigned 8 bit integer.
typedef unsigned short UI16;		// Unsigned 16 bit integer.
typedef unsigned int UI32;			// Unsigned 32 bit integer.
typedef unsigned long long UI64;	// Unsigned 64 bit integer.

typedef signed char SI8;			// Signed 8 bit integer.
typedef signed short SI16;			// Signed 16 bit integer.
typedef signed int SI32;			// Signed 32 bit integer.
typedef signed long long SI64;		// Signed 64 bit integer.

typedef signed char I8;				// 8 bit integer.
typedef short I16;					// 16 bit integer.
typedef int I32;					// 32 bit integer.
typedef long long I64;				// 64 bit integer.

#define TEXT(text)  L##text
typedef std::string String;
typedef std::wstring WString;
typedef wchar_t wchar;

typedef UI8 BYTE;					// Byte of data.
typedef BYTE* BPTR;					// Byte pointer.

#define INSERT_INTO_VECTOR(vector, data)	vector.insert(vector.end(), data)
