// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Maths/Vector2F.hpp"

namespace Flint
{
	float* Vector2F::Decomompose()
	{
		return mStorage;
	}

	const float* Vector2F::Decomompose() const
	{
		return mStorage;
	}

	float& Vector2F::At(const UI32 index)
	{
		return mStorage[index];
	}

	const float Vector2F::At(const UI32 index) const
	{
		return mStorage[index];
	}

	Vector2F::operator float* ()
	{
		return Decomompose();
	}

	Vector2F::operator const float* () const
	{
		return Decomompose();
	}

	float& Vector2F::operator[](const UI32 index)
	{
		return At(index);
	}

	const float Vector2F::operator[](const UI32 index) const
	{
		return At(index);
	}

	Vector2F Vector2F::operator+(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] + other[0], mStorage[1] + other[1]);
	}

	Vector2F Vector2F::operator-(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] - other[0], mStorage[1] - other[1]);
	}

	Vector2F Vector2F::operator*(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] * other[0], mStorage[1] * other[1]);
	}

	Vector2F Vector2F::operator/(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] / other[0], mStorage[1] / other[1]);
	}

	Vector2F& Vector2F::operator+=(const Vector2F& other)
	{
		(*this) = (*this) + other;
		return *this;
	}

	Vector2F& Vector2F::operator-=(const Vector2F& other)
	{
		(*this) = (*this) - other;
		return *this;
	}

	Vector2F& Vector2F::operator*=(const Vector2F& other)
	{
		(*this) = (*this) * other;
		return *this;
	}

	Vector2F& Vector2F::operator/=(const Vector2F& other)
	{
		(*this) = (*this) / other;
		return *this;
	}
}
