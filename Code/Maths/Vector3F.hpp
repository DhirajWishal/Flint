// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Vector2F.hpp"

namespace Flint
{
	/**
	 * Flint vector 3 F (float) class.
	 */
	class Vector3F final
	{
	public:
		/**
		 * Default constructor.
		 */
		Vector3F() = default;

		/**
		 * Constructor.
		 *
		 * @param value The value to set.
		 */
		constexpr Vector3F(float value) { mStorage[0] = value; mStorage[1] = value; mStorage[2] = value; }

		/**
		 * Constructor.
		 *
		 * @param x The x argument.
		 * @param y The y argument.
		 * @param z The z argument.
		 */
		constexpr Vector3F(float x, float y, float z) { mStorage[0] = x; mStorage[1] = y; mStorage[2] = z; }

		/**
		 * Constructor.
		 *
		 * @param vec2 The Vector2F.
		 * @param z The z argument.
		 */
		constexpr Vector3F(const Vector2F vec2, float z) { mStorage[0] = vec2[0]; mStorage[1] = vec2[1]; mStorage[2] = z; }

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
		Vector3F operator+(const Vector3F& other) const;

		/**
		 * Subtract operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector3F operator-(const Vector3F& other) const;

		/**
		 * Multiply operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector3F operator*(const Vector3F& other) const;

		/**
		 * Divide operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector3F operator/(const Vector3F& other) const;

		/**
		 * Add assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector3F& operator+=(const Vector3F& other);

		/**
		 * Subtract assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector3F& operator-=(const Vector3F& other);

		/**
		 * Multiply assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector3F& operator*=(const Vector3F& other);

		/**
		 * Divide assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector3F& operator/=(const Vector3F& other);

		/**
		 * Is equal to operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector3F operator==(const Vector3F& other) const;

		/**
		 * Is not equal to operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector3F operator!=(const Vector3F& other) const;

		/**
		 * Is less than operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector3F operator<(const Vector3F& other) const;

		/**
		 * Is less than or equal to operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector3F operator<=(const Vector3F& other) const;

		/**
		 * Is greater than operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector3F operator>(const Vector3F& other) const;

		/**
		 * Is greater than or equal to operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector
		 */
		Vector3F operator>=(const Vector3F& other) const;

		/**
		 * Logical AND operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector3F operator&&(const Vector3F& other) const;

		/**
		 * Logical OR operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector3F operator||(const Vector3F& other) const;

		/**
		 * Logical XOR operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector3F operator^(const Vector3F& other) const;

		/**
		 * Logical NOT operator.
		 *
		 * @return The resulting vector.
		 */
		Vector3F operator!() const;

	public:
		/**
		 * Compute the power two vectors.
		 *
		 * @param lhs The left hand side argument.
		 * @param rhs The right hand side argument.
		 * @return The resulting vector.
		 */
		static Vector3F Power(const Vector3F lhs, const Vector3F rhs);

		/**
		 * Compute the square root of a vector.
		 *
		 * @param vec The vector to get the square root of.
		 * @return The resulting vector.
		 */
		static Vector3F SquareRoot(const Vector3F vec);

		/**
		 * Compute the radian sin of a vector.
		 *
		 * @param vec The vector to get the sin of.
		 * @return The resulting vector.
		 */
		static Vector3F SinRad(const Vector3F vec);

		/**
		 * Compute the radian cos of a vector.
		 *
		 * @param vec The vector to get the cos of.
		 * @return The resulting vector.
		 */
		static Vector3F CosRad(const Vector3F vec);

		/**
		 * Compute the radian tan of a vector.
		 *
		 * @param vec The vector to get the tan of.
		 * @return The resulting vector.
		 */
		static Vector3F TanRad(const Vector3F vec);

		/**
		 * Compute the degree sin of a vector.
		 *
		 * @param vec The vector to get the sin of.
		 * @return The resulting vector.
		 */
		static Vector3F SinDeg(const Vector3F vec);

		/**
		 * Compute the degree cos of a vector.
		 *
		 * @param vec The vector to get the cos of.
		 * @return The resulting vector.
		 */
		static Vector3F CosDeg(const Vector3F vec);

		/**
		 * Compute the degree tan of a vector.
		 *
		 * @param vec The vector to get the tan of.
		 * @return The resulting vector.
		 */
		static Vector3F TanDeg(const Vector3F vec);

		/**
		 * Compute the inverse radian sin of a vector.
		 *
		 * @param vec The vector to get the sin of.
		 * @return The resulting vector.
		 */
		static Vector3F InverseSinRad(const Vector3F vec);

		/**
		 * Compute the inverse radian cos of a vector.
		 *
		 * @param vec The vector to get the cos of.
		 * @return The resulting vector.
		 */
		static Vector3F InverseCosRad(const Vector3F vec);

		/**
		 * Compute the inverse radian tan of a vector.
		 *
		 * @param vec The vector to get the tan of.
		 * @return The resulting vector.
		 */
		static Vector3F InverseTanRad(const Vector3F vec);

		/**
		 * Compute the hypotenuse of a right angled triangle using two vectors.
		 *
		 * @param lhs The left hand side argument.
		 * @param rhs The right hand side argument.
		 * @return The resulting vector.
		 */
		static Vector3F Hypotenuse(const Vector3F lhs, const Vector3F rhs);

	private:
		float mStorage[3] = {};
	};
}