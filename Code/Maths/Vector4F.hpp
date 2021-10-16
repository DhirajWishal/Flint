// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Vector3F.hpp"

namespace Flint
{
	/**
	 * Flint vector 4 F (float) class.
	 */
	class Vector4F final
	{
	public:
		/**
		 * Default constructor.
		 */
		Vector4F() = default;

		/**
		 * Constructor.
		 *
		 * @param value The value to set.
		 */
		constexpr Vector4F(float value) { mStorage[0] = value; mStorage[1] = value; mStorage[2] = value; }

		/**
		 * Constructor.
		 *
		 * @param x The x argument.
		 * @param y The y argument.
		 * @param z The z argument.
		 * @param w The w argument.
		 */
		constexpr Vector4F(float x, float y, float z, float w) { mStorage[0] = x; mStorage[1] = y; mStorage[2] = z; mStorage[3] = w; }

		/**
		 * Constructor.
		 *
		 * @param x The first Vector2F.
		 * @param y The second Vector2F.
		 */
		constexpr Vector4F(const Vector2F x, const Vector2F y) { mStorage[0] = x[0]; mStorage[1] = x[1]; mStorage[2] = y[0]; mStorage[3] = y[1]; }

		/**
		 * Constructor.
		 *
		 * @param vec3 The Vector2F.
		 * @param w The w argument.
		 */
		constexpr Vector4F(const Vector3F vec3, float w) { mStorage[0] = vec3[0]; mStorage[1] = vec3[1]; mStorage[2] = vec3[2]; mStorage[3] = w; }

		/**
		 * Decompose the vector.
		 * This will return its memory address.
		 *
		 * @return The float pointer.
		 */
		float* Decompose();

		/**
		 * Decompose the vector.
		 * This will return its memory address.
		 *
		 * @return The float pointer.
		 */
		const float* Decompose() const;

		/**
		 * Get the element value at a given index.
		 *
		 * @param index The index of the value to access.
		 * @return The value.
		 */
		float& At(const UI32 index);

		/**
		 * Get the element value at a given index.
		 *
		 * @param index The index of the value to access.
		 * @return The value.
		 */
		const float At(const UI32 index) const;

	public:
		/**
		 * Implicit float pointer operator.
		 *
		 * @return The decomposed pointer.
		 */
		operator float* ();

		/**
		 * Implicit float pointer operator.
		 *
		 * @return The decomposed pointer.
		 */
		operator const float* () const;

		/**
		 * Explicit boolean operator.
		 * This will return true if at least one element is greater than 0.
		 *
		 * @return The boolean value.
		 */
		explicit operator bool() const;

		/**
		 * Subscript operator.
		 *
		 * @param index The index to access.
		 * @return The value at the given index.
		 */
		float& operator[](const UI32 index);

		/**
		 * Subscript operator.
		 *
		 * @param index The index to access.
		 * @return The value at the given index.
		 */
		const float operator[](const UI32 index) const;

		/**
		 * Add operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F operator+(const Vector4F& other) const;

		/**
		 * Subtract operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F operator-(const Vector4F& other) const;

		/**
		 * Multiply operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F operator*(const Vector4F& other) const;

		/**
		 * Divide operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F operator/(const Vector4F& other) const;

		/**
		 * Add assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F& operator+=(const Vector4F& other);

		/**
		 * Subtract assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F& operator-=(const Vector4F& other);

		/**
		 * Multiply assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F& operator*=(const Vector4F& other);

		/**
		 * Divide assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F& operator/=(const Vector4F& other);

		/**
		 * Is equal to operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector4F operator==(const Vector4F& other) const;

		/**
		 * Is not equal to operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector4F operator!=(const Vector4F& other) const;

		/**
		 * Is less than operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector4F operator<(const Vector4F& other) const;

		/**
		 * Is less than or equal to operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector4F operator<=(const Vector4F& other) const;

		/**
		 * Is greater than operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector4F operator>(const Vector4F& other) const;

		/**
		 * Is greater than or equal to operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector4F operator>=(const Vector4F& other) const;

		/**
		 * Logical AND operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F operator&&(const Vector4F& other) const;

		/**
		 * Logical OR operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F operator||(const Vector4F& other) const;

		/**
		 * Logical XOR operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector4F operator^(const Vector4F& other) const;

		/**
		 * Logical NOT operator.
		 *
		 * @return The resulting vector.
		 */
		Vector4F operator!() const;

	public:
		/**
		 * Compute the power two vectors.
		 *
		 * @param lhs The left hand side argument.
		 * @param rhs The right hand side argument.
		 * @return The resulting vector.
		 */
		static Vector4F Power(const Vector4F lhs, const Vector4F rhs);

		/**
		 * Compute the square root of a vector.
		 *
		 * @param vec The vector to get the square root of.
		 * @return The resulting vector.
		 */
		static Vector4F SquareRoot(const Vector4F vec);

		/**
		 * Compute the radian sin of a vector.
		 *
		 * @param vec The vector to get the sin of.
		 * @return The resulting vector.
		 */
		static Vector4F SinRad(const Vector4F vec);

		/**
		 * Compute the radian cos of a vector.
		 *
		 * @param vec The vector to get the cos of.
		 * @return The resulting vector.
		 */
		static Vector4F CosRad(const Vector4F vec);

		/**
		 * Compute the radian tan of a vector.
		 *
		 * @param vec The vector to get the tan of.
		 * @return The resulting vector.
		 */
		static Vector4F TanRad(const Vector4F vec);

		/**
		 * Compute the degree sin of a vector.
		 *
		 * @param vec The vector to get the sin of.
		 * @return The resulting vector.
		 */
		static Vector4F SinDeg(const Vector4F vec);

		/**
		 * Compute the degree cos of a vector.
		 *
		 * @param vec The vector to get the cos of.
		 * @return The resulting vector.
		 */
		static Vector4F CosDeg(const Vector4F vec);

		/**
		 * Compute the degree tan of a vector.
		 *
		 * @param vec The vector to get the tan of.
		 * @return The resulting vector.
		 */
		static Vector4F TanDeg(const Vector4F vec);

		/**
		 * Compute the inverse radian sin of a vector.
		 *
		 * @param vec The vector to get the sin of.
		 * @return The resulting vector.
		 */
		static Vector4F InverseSinRad(const Vector4F vec);

		/**
		 * Compute the inverse radian cos of a vector.
		 *
		 * @param vec The vector to get the cos of.
		 * @return The resulting vector.
		 */
		static Vector4F InverseCosRad(const Vector4F vec);

		/**
		 * Compute the inverse radian tan of a vector.
		 *
		 * @param vec The vector to get the tan of.
		 * @return The resulting vector.
		 */
		static Vector4F InverseTanRad(const Vector4F vec);

		/**
		 * Compute the hypotenuse of a right angled triangle using two vectors.
		 *
		 * @param lhs The left hand side argument.
		 * @param rhs The right hand side argument.
		 * @return The resulting vector.
		 */
		static Vector4F Hypotenuse(const Vector4F lhs, const Vector4F rhs);

	private:
		float mStorage[4] = {};
	};
}