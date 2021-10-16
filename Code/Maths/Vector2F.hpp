// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"

namespace Flint
{
	/**
	 * Flint vector 2 F (float) class.
	 */
	class Vector2F final
	{
	public:
		/**
		 * Default constructor.
		 */
		Vector2F() = default;

		/**
		 * Constructor.
		 *
		 * @param x The x argument.
		 * @param y The y argument.
		 */
		constexpr Vector2F(float x, float y) { mStorage[0] = x; mStorage[1] = y; }

		/**
		 * Decompose the vector.
		 * This will return its memory address.
		 *
		 * @return The float pointer.
		 */
		float* Decomompose();

		/**
		 * Decompose the vector.
		 * This will return its memory address.
		 *
		 * @return The float pointer.
		 */
		const float* Decomompose() const;

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
		Vector2F operator+(const Vector2F& other) const;

		/**
		 * Subtract operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector2F operator-(const Vector2F& other) const;

		/**
		 * Multiply operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector2F operator*(const Vector2F& other) const;

		/**
		 * Divide operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector2F operator/(const Vector2F& other) const;

		/**
		 * Add assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector2F& operator+=(const Vector2F& other);

		/**
		 * Subtract assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector2F& operator-=(const Vector2F& other);

		/**
		 * Multiply assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector2F& operator*=(const Vector2F& other);

		/**
		 * Divide assignment operator.
		 *
		 * @param other The other vector.
		 * @return The resulting vector.
		 */
		Vector2F& operator/=(const Vector2F& other);

	private:
		float mStorage[2] = {};
	};
}