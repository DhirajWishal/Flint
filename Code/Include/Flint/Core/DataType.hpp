// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Macros.hpp"
#include <cstdint>

typedef uint8_t								UI8;
typedef uint16_t							UI16;
typedef uint32_t							UI32;
typedef uint64_t							UI64;

typedef int8_t								I8;
typedef int16_t								I16;
typedef int32_t								I32;
typedef int64_t								I64;

typedef wchar_t								wchar;

typedef UI8									BYTE;

namespace Flint
{
	/**
	 * Flint extent 2D structure.
	 */
	struct FLINT_API FExtent2D
	{
		FExtent2D() = default;
		FExtent2D(UI32 width, UI32 height) : mWidth(width), mHeight(height) {}

		UI32 mWidth = 0, mHeight = 0;
	};

	/**
	 * Flint extent 3D structure.
	 */
	struct FLINT_API FExtent3D
	{
		FExtent3D() = default;
		FExtent3D(UI32 width, UI32 height, UI32 depth) : mWidth(width), mHeight(height), mDepth(depth) {}

		UI32 mWidth = 0, mHeight = 0, mDepth = 0;
	};
}