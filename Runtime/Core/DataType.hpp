// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Macros.hpp"

#include <cstdint>
#include <limits>

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
		constexpr FColor4D() = default;
		constexpr FColor4D(float value) : mRed(value), mGreen(value), mBlue(value), mAlpha(value) {}
		constexpr FColor4D(float red, float green, float blue, float alpha) : mRed(red), mGreen(green), mBlue(blue), mAlpha(alpha) {}

		float mRed = 0.0f;
		float mGreen = 0.0f;
		float mBlue = 0.0f;
		float mAlpha = 0.0f;
	};

	using FBox2D = FExtent2D<uint32_t>;
	using FBox3D = FExtent3D<uint32_t>;

	template<class Type> constexpr bool operator==(const FExtent2D<Type>& lhs, const FExtent2D<Type>& rhs) { return lhs.X == rhs.X && lhs.Y == rhs.Y; }
	template<class Type> constexpr bool operator!=(const FExtent2D<Type>& lhs, const FExtent2D<Type>& rhs) { return lhs.X != rhs.X || lhs.Y != rhs.Y; }
	template<class Type> constexpr bool operator==(const FExtent3D<Type>& lhs, const FExtent3D<Type>& rhs) { return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z; }
	template<class Type> constexpr bool operator!=(const FExtent3D<Type>& lhs, const FExtent3D<Type>& rhs) { return lhs.X != rhs.X || lhs.Y != rhs.Y || lhs.Z != rhs.Z; }

	constexpr float CreateColor256(const float color) { return color / 256.0f; }
	constexpr uint64_t BitShiftLeft(const uint64_t count) { return static_cast<uint64_t>(1) << count; }
	constexpr uint64_t BitShiftRight(const uint64_t count) { return static_cast<uint64_t>(1) << count; }
	inline std::filesystem::path NormalizePath(std::filesystem::path path) { return path.make_preferred(); }
}