// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Maths/Vector4F.hpp"

#include <immintrin.h>

namespace Flint
{
	float* Vector4F::Decompose()
	{
		return mStorage;
	}

	const float* Vector4F::Decompose() const
	{
		return mStorage;
	}

	float& Vector4F::At(const UI32 index)
	{
		return mStorage[index];
	}

	const float Vector4F::At(const UI32 index) const
	{
		return mStorage[index];
	}

	Vector4F::operator float* ()
	{
		return Decompose();
	}

	Vector4F::operator const float* () const
	{
		return Decompose();
	}

	Vector4F::operator bool() const
	{
		return (mStorage[0] > 0.0f)
			|| (mStorage[1] > 0.0f)
			|| (mStorage[2] > 0.0f)
			|| (mStorage[3] > 0.0f);
	}

	float& Vector4F::operator[](const UI32 index)
	{
		return At(index);
	}

	const float Vector4F::operator[](const UI32 index) const
	{
		return At(index);
	}

	Vector4F Vector4F::operator+(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_add_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator-(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_sub_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator*(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_mul_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator/(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_div_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F& Vector4F::operator+=(const Vector4F& other)
	{
		(*this) = (*this) + other;
		return *this;
	}

	Vector4F& Vector4F::operator-=(const Vector4F& other)
	{
		(*this) = (*this) - other;
		return *this;
	}

	Vector4F& Vector4F::operator*=(const Vector4F& other)
	{
		(*this) = (*this) * other;
		return *this;
	}

	Vector4F& Vector4F::operator/=(const Vector4F& other)
	{
		(*this) = (*this) / other;
		return *this;
	}

	Vector4F Vector4F::operator==(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_cmpeq_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator!=(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_cmpneq_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator<(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_cmplt_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator<=(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_cmple_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator>(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_cmpgt_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator>=(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_cmpge_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator&&(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_and_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator||(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_or_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator^(const Vector4F& other) const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_xor_ps(_mm_load_ps(*this), _mm_load_ps(other)));

		return result;
	}

	Vector4F Vector4F::operator!() const
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_andnot_ps(_mm_load_ps(*this), _mm_load_ps(*this)));

		return result;
	}

	Vector4F Vector4F::Power(const Vector4F lhs, const Vector4F rhs)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_pow_ps(_mm_load_ps(lhs), _mm_load_ps(rhs)));

		return result;
	}

	Vector4F Vector4F::SquareRoot(const Vector4F vec)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_sqrt_ps(_mm_load_ps(vec)));

		return result;
	}

	Vector4F Vector4F::SinRad(const Vector4F vec)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_sin_ps(_mm_load_ps(vec)));

		return result;
	}

	Vector4F Vector4F::CosRad(const Vector4F vec)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_cos_ps(_mm_load_ps(vec)));

		return result;
	}

	Vector4F Vector4F::TanRad(const Vector4F vec)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_tan_ps(_mm_load_ps(vec)));

		return result;
	}

	Vector4F Vector4F::SinDeg(const Vector4F vec)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_sind_ps(_mm_load_ps(vec)));

		return result;
	}

	Vector4F Vector4F::CosDeg(const Vector4F vec)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_cosd_ps(_mm_load_ps(vec)));

		return result;
	}

	Vector4F Vector4F::TanDeg(const Vector4F vec)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_tand_ps(_mm_load_ps(vec)));

		return result;
	}

	Vector4F Vector4F::InverseSinRad(const Vector4F vec)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_asin_ps(_mm_load_ps(vec)));

		return result;
	}

	Vector4F Vector4F::InverseCosRad(const Vector4F vec)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_acos_ps(_mm_load_ps(vec)));

		return result;
	}

	Vector4F Vector4F::InverseTanRad(const Vector4F vec)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_atan_ps(_mm_load_ps(vec)));

		return result;
	}

	Vector4F Vector4F::Hypotenuse(const Vector4F lhs, const Vector4F rhs)
	{
		Vector4F result = {};
		_mm_store_ps(result, _mm_hypot_ps(_mm_load_ps(lhs), _mm_load_ps(rhs)));

		return result;
	}
}
