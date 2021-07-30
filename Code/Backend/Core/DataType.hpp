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
	 *
	 * @tparam Type: The type of the struct.
	 */
	template<class Type>
	struct FExtent2D
	{
		FExtent2D() = default;
		FExtent2D(Type width, Type height) : mWidth(width), mHeight(height) {}

	public:
		union
		{
			struct
			{
				Type mWidth;
				Type mHeight;
			};

			struct
			{
				Type X;
				Type Y;
			};
		};
	};

	/**
	 * Flint extent 3D structure.
	 *
	 * @tparam Type: The type of the struct.
	 */
	template<class Type>
	struct FExtent3D
	{
		FExtent3D() = default;
		FExtent3D(Type width, Type height, Type depth) : mWidth(width), mHeight(height), mDepth(depth) {}

	public:
		union
		{
			struct
			{
				Type mWidth;
				Type mHeight;
				Type mDepth;
			};

			struct
			{
				Type X;
				Type Y;
				Type Z;
			};
		};
	};

	/**
	 * Flint color 4D structure.
	 */
	struct FColor4D
	{
		FColor4D() = default;
		FColor4D(float red, float green, float blue, float alpha)
			: mRed(red), mGreen(green), mBlue(blue), mAlpha(alpha) {}

		float mRed = 0.0f, mGreen = 0.0f, mBlue = 0.0f, mAlpha = 0.0f;
	};

	typedef FExtent2D<UI32> FBox2D;
	typedef FExtent3D<UI32> FBox3D;
}