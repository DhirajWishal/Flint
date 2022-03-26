// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Macros.hpp"

#include <cstdint>
#include <limits>

namespace Flint
{
	using uint8 = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;

	using int8 = int8_t;
	using int16 = int16_t;
	using int32 = int32_t;
	using int64 = int64_t;

	using wchar = wchar_t;

	using BYTE = uint8;

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
		constexpr FColor4D() = default;
		constexpr FColor4D(float value) : mRed(value), mGreen(value), mBlue(value), mAlpha(value) {}
		constexpr FColor4D(float red, float green, float blue, float alpha) : mRed(red), mGreen(green), mBlue(blue), mAlpha(alpha) {}

		float mRed = 0.0f;
		float mGreen = 0.0f;
		float mBlue = 0.0f;
		float mAlpha = 0.0f;
	};

	using FBox2D = FExtent2D<uint32>;
	using FBox3D = FExtent3D<uint32>;

	template<class Type> constexpr bool operator==(const FExtent2D<Type>& lhs, const FExtent2D<Type>& rhs) { return lhs.X == rhs.X && lhs.Y == rhs.Y; }
	template<class Type> constexpr bool operator!=(const FExtent2D<Type>& lhs, const FExtent2D<Type>& rhs) { return lhs.X != rhs.X || lhs.Y != rhs.Y; }
	template<class Type> constexpr bool operator==(const FExtent3D<Type>& lhs, const FExtent3D<Type>& rhs) { return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z; }
	template<class Type> constexpr bool operator!=(const FExtent3D<Type>& lhs, const FExtent3D<Type>& rhs) { return lhs.X != rhs.X || lhs.Y != rhs.Y || lhs.Z != rhs.Z; }

	constexpr float CreateColor256(const float color) { return color / 256.0f; }
	constexpr uint64 BitShiftLeft(const uint64 count) { return static_cast<uint64>(1) << count; }
	constexpr uint64 BitShiftRight(const uint64 count) { return static_cast<uint64>(1) << count; }
	inline std::filesystem::path NormalizePath(std::filesystem::path path) { return path.make_preferred(); }
}