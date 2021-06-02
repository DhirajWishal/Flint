// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Interface.h"

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

#define DEFINE_HANDLE_UI8(handle)			enum class handle : UI8 {}
#define DEFINE_HANDLE_UI16(handle)			enum class handle : UI16 {}
#define DEFINE_HANDLE_UI32(handle)			enum class handle : UI32 {}
#define DEFINE_HANDLE_UI64(handle)			enum class handle : UI64 {}

namespace Flint
{
	/**
	 * Flint extent 2D structure.
	 */
	struct FLINT_API FExtent2D
	{
		FExtent2D() = default;
		FExtent2D(UI32 x, UI32 y) : x(x), y(y) {}

		union
		{
			struct { UI32 mWidth, mHeight; };
			struct { UI32 width, height; };
			struct { UI32 x, y; };
			struct { UI32 a, b; };
		};
	};

	/**
	 * Flint extent 3D structure.
	 */
	struct FLINT_API FExtent3D
	{
		FExtent3D() = default;
		FExtent3D(UI32 x, UI32 y, UI32 z) : x(x), y(y), z(z) {}

		union
		{
			struct { UI32 mWidth, mHeight, mDepth; };
			struct { UI32 width, height, deth; };
			struct { UI32 x, y, z; };
			struct { UI32 a, b, c; };
		};
	};

	enum class ColorComponent : UI8 {
		RED,
		GREEN,
		BLUE,
		ALPHA
	};

	struct FFormat1
	{
		FFormat1(ColorComponent component) : mComponent(component) {}
		ColorComponent mComponent = ColorComponent::ALPHA;
	};

	struct FFormat2
	{
		FFormat2(ColorComponent component1, ColorComponent component2) : mComponent(component) {}
		
		ColorComponent mComponentOne = ColorComponent::ALPHA;
		ColorComponent mComponentTwo = ColorComponent::ALPHA;
	};
}
