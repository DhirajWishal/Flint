// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Macros.hpp"

#include <cstdint>
#include <limits>

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
	 * @tparam Type The type of the struct.
	 */
	template<class Type>
	struct FExtent2D
	{
		constexpr FExtent2D() = default;
		constexpr FExtent2D(Type value) : mWidth(value), mHeight(value) {}
		constexpr FExtent2D(Type width, Type height) : mWidth(width), mHeight(height) {}

		constexpr bool IsZero() const { return mWidth == Type() || mHeight == Type(); }

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
	 * @tparam Type The type of the struct.
	 */
	template<class Type>
	struct FExtent3D
	{
		constexpr FExtent3D() = default;
		constexpr FExtent3D(Type value) : mWidth(value), mHeight(value), mDepth(value) {}
		constexpr FExtent3D(Type width, Type height, Type depth) : mWidth(width), mHeight(height), mDepth(depth) {}

		constexpr bool IsZero() const { return mWidth == Type() || mHeight == Type() || mDepth == Type(); }

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
		FColor4D(float value) : mRed(value), mGreen(value), mBlue(value), mAlpha(value) {}
		FColor4D(float red, float green, float blue, float alpha) : mRed(red), mGreen(green), mBlue(blue), mAlpha(alpha) {}

		float mRed = 0.0f, mGreen = 0.0f, mBlue = 0.0f, mAlpha = 0.0f;
	};

	typedef FExtent2D<UI32> FBox2D;
	typedef FExtent3D<UI32> FBox3D;

	template<class Type> constexpr bool operator==(const FExtent2D<Type>& lhs, const FExtent2D<Type>& rhs) { return lhs.X == rhs.X && lhs.Y == rhs.Y; }
	template<class Type> constexpr bool operator!=(const FExtent2D<Type>& lhs, const FExtent2D<Type>& rhs) { return lhs.X != rhs.X || lhs.Y != rhs.Y; }
	template<class Type> constexpr bool operator==(const FExtent3D<Type>& lhs, const FExtent3D<Type>& rhs) { return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z; }
	template<class Type> constexpr bool operator!=(const FExtent3D<Type>& lhs, const FExtent3D<Type>& rhs) { return lhs.X != rhs.X || lhs.Y != rhs.Y || lhs.Z != rhs.Z; }

	constexpr float CreateColor256(const float color) { return color / 256.0f; }
	constexpr UI64 BitShiftLeft(const UI64 count) { return static_cast<UI64>(1) << count; }
	constexpr UI64 BitShiftRight(const UI64 count) { return static_cast<UI64>(1) << count; }
	inline std::filesystem::path NormalizePath(std::filesystem::path path) { return path.make_preferred(); }
}