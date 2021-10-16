// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Maths/Vector2F.hpp"

#include <cmath>

namespace Flint
{
	namespace Helpers
	{
		float ToRadians(float degrees) { return degrees * (3.14159f / 180.0f); }
	}

	float* Vector2F::Decompose()
	{
		return mStorage;
	}

	const float* Vector2F::Decompose() const
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
		return Decompose();
	}

	Vector2F::operator const float* () const
	{
		return Decompose();
	}

	Vector2F::operator bool() const
	{
		return (mStorage[0] > 0.0f)
			|| (mStorage[1] > 0.0f);
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

	Vector2F Vector2F::operator==(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] == other[0], mStorage[1] == other[1]);
	}

	Vector2F Vector2F::operator!=(const Vector2F& other) const
	{
		return mStorage[0] != other[0] || mStorage[1] != other[1];
	}

	Vector2F Vector2F::operator<(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] < other[0], mStorage[1] < other[1]);
	}

	Vector2F Vector2F::operator<=(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] <= other[0], mStorage[1] <= other[1]);
	}

	Vector2F Vector2F::operator>(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] > other[0], mStorage[1] > other[1]);
	}

	Vector2F Vector2F::operator>=(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] >= other[0], mStorage[1] >= other[1]);
	}

	Vector2F Vector2F::operator&&(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] && other[0], mStorage[1] && other[1]);
	}

	Vector2F Vector2F::operator||(const Vector2F& other) const
	{
		return Vector2F(mStorage[0] || other[0], mStorage[1] || other[1]);
	}

	Vector2F Vector2F::operator^(const Vector2F& other) const
	{
		return Vector2F(static_cast<float>(static_cast<I32>(mStorage[0]) ^ static_cast<I32>(other[0])), static_cast<float>(static_cast<I32>(mStorage[1]) ^ static_cast<I32>(other[1])));
	}

	Vector2F Vector2F::operator!() const
	{
		return Vector2F(!mStorage[0], !mStorage[1]);
	}

	Vector2F Vector2F::Power(const Vector2F lhs, const Vector2F rhs)
	{
		return Vector2F(std::pow(lhs[0], rhs[0]), std::pow(lhs[1], rhs[1]));
	}

	Vector2F Vector2F::SquareRoot(const Vector2F vec)
	{
		return Vector2F(std::sqrt(vec[0]), std::sqrt(vec[1]));
	}

	Vector2F Vector2F::SinRad(const Vector2F vec)
	{
		return Vector2F(std::sin(vec[0]), std::sin(vec[1]));
	}

	Vector2F Vector2F::CosRad(const Vector2F vec)
	{
		return Vector2F(std::cos(vec[0]), std::cos(vec[1]));
	}

	Vector2F Vector2F::TanRad(const Vector2F vec)
	{
		return Vector2F(std::tan(vec[0]), std::tan(vec[1]));
	}

	Vector2F Vector2F::SinDeg(const Vector2F vec)
	{
		return Vector2F(std::sin(Helpers::ToRadians(vec[0])), std::sin(Helpers::ToRadians(vec[1])));
	}

	Vector2F Vector2F::CosDeg(const Vector2F vec)
	{
		return Vector2F(std::cos(Helpers::ToRadians(vec[0])), std::cos(Helpers::ToRadians(vec[1])));
	}

	Vector2F Vector2F::TanDeg(const Vector2F vec)
	{
		return Vector2F(std::tan(Helpers::ToRadians(vec[0])), std::tan(Helpers::ToRadians(vec[1])));
	}

	Vector2F Vector2F::InverseSinRad(const Vector2F vec)
	{
		return Vector2F(std::asin(vec[0]), std::asin(vec[1]));
	}

	Vector2F Vector2F::InverseCosRad(const Vector2F vec)
	{
		return Vector2F(std::acos(vec[0]), std::acos(vec[1]));
	}

	Vector2F Vector2F::InverseTanRad(const Vector2F vec)
	{
		return Vector2F(std::atan(vec[0]), std::atan(vec[1]));
	}

	Vector2F Vector2F::Hypotenuse(const Vector2F lhs, const Vector2F rhs)
	{
		return Vector2F(std::hypot(lhs[0], rhs[0]), std::hypot(lhs[1], rhs[1]));
	}
}
