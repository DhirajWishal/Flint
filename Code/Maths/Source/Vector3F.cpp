// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Maths/Vector3F.hpp"

#include <immintrin.h>

namespace Flint
{
	namespace Helpers
	{
		struct Container
		{
			float mFirstArg[4] = {};
			float mSecondArg[4] = {};
			float mResult[4] = {};

			void ToFirstArg(const Vector3F vec) { std::copy_n(vec.Decompose(), 3, mFirstArg); }
			void ToSecondArg(const Vector3F vec) { std::copy_n(vec.Decompose(), 3, mSecondArg); }
			void ToResult(const __m128 vec) { _mm_store_ps(mResult, vec); }
			__m128 GetFirstIntrinsic() const { return _mm_load_ps(mFirstArg); }
			__m128 GetSecondIntrinsic() const { return _mm_load_ps(mSecondArg); }

			Vector3F GetVector3F() const
			{
				Vector3F vec = {};
				std::copy_n(mResult, 3, vec.Decompose());

				return vec;
			}
		};
	}

	float* Vector3F::Decompose()
	{
		return mStorage;
	}

	const float* Vector3F::Decompose() const
	{
		return mStorage;
	}

	float& Vector3F::At(const UI32 index)
	{
		return mStorage[index];
	}

	const float Vector3F::At(const UI32 index) const
	{
		return mStorage[index];
	}

	Vector3F::operator float* ()
	{
		return Decompose();
	}

	Vector3F::operator const float* () const
	{
		return Decompose();
	}

	Vector3F::operator bool() const
	{
		return (mStorage[0] > 0.0f)
			|| (mStorage[1] > 0.0f)
			|| (mStorage[2] > 0.0f);
	}

	float& Vector3F::operator[](const UI32 index)
	{
		return At(index);
	}

	const float Vector3F::operator[](const UI32 index) const
	{
		return At(index);
	}

	Vector3F Vector3F::operator+(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_add_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator-(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_sub_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator*(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_mul_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator/(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_div_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F& Vector3F::operator+=(const Vector3F& other)
	{
		(*this) = (*this) + other;
		return *this;
	}

	Vector3F& Vector3F::operator-=(const Vector3F& other)
	{
		(*this) = (*this) - other;
		return *this;
	}

	Vector3F& Vector3F::operator*=(const Vector3F& other)
	{
		(*this) = (*this) * other;
		return *this;
	}

	Vector3F& Vector3F::operator/=(const Vector3F& other)
	{
		(*this) = (*this) / other;
		return *this;
	}

	Vector3F Vector3F::operator==(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_cmpeq_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator!=(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_cmpneq_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator<(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_cmplt_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator<=(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_cmple_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator>(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_cmpgt_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator>=(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_cmpge_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator&&(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_and_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator||(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_or_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator^(const Vector3F& other) const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToFirstArg(other);
		container.ToResult(_mm_xor_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::operator!() const
	{
		Helpers::Container container = {};
		container.ToFirstArg(*this);
		container.ToResult(_mm_andnot_ps(container.GetFirstIntrinsic(), container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::Power(const Vector3F lhs, const Vector3F rhs)
	{
		Helpers::Container container = {};
		container.ToFirstArg(lhs);
		container.ToFirstArg(rhs);
		container.ToResult(_mm_pow_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::SquareRoot(const Vector3F vec)
	{
		Helpers::Container container = {};
		container.ToFirstArg(vec);
		container.ToResult(_mm_sqrt_ps(container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::SinRad(const Vector3F vec)
	{
		Helpers::Container container = {};
		container.ToFirstArg(vec);
		container.ToResult(_mm_sin_ps(container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::CosRad(const Vector3F vec)
	{
		Helpers::Container container = {};
		container.ToFirstArg(vec);
		container.ToResult(_mm_cos_ps(container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::TanRad(const Vector3F vec)
	{
		Helpers::Container container = {};
		container.ToFirstArg(vec);
		container.ToResult(_mm_tan_ps(container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::SinDeg(const Vector3F vec)
	{
		Helpers::Container container = {};
		container.ToFirstArg(vec);
		container.ToResult(_mm_sind_ps(container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::CosDeg(const Vector3F vec)
	{
		Helpers::Container container = {};
		container.ToFirstArg(vec);
		container.ToResult(_mm_cosd_ps(container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::TanDeg(const Vector3F vec)
	{
		Helpers::Container container = {};
		container.ToFirstArg(vec);
		container.ToResult(_mm_tand_ps(container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::InverseSinRad(const Vector3F vec)
	{
		Helpers::Container container = {};
		container.ToFirstArg(vec);
		container.ToResult(_mm_acos_ps(container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::InverseCosRad(const Vector3F vec)
	{
		Helpers::Container container = {};
		container.ToFirstArg(vec);
		container.ToResult(_mm_asin_ps(container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::InverseTanRad(const Vector3F vec)
	{
		Helpers::Container container = {};
		container.ToFirstArg(vec);
		container.ToResult(_mm_atan_ps(container.GetFirstIntrinsic()));
		return container.GetVector3F();
	}

	Vector3F Vector3F::Hypotenuse(const Vector3F lhs, const Vector3F rhs)
	{
		Helpers::Container container = {};
		container.ToFirstArg(lhs);
		container.ToFirstArg(rhs);
		container.ToResult(_mm_hypot_ps(container.GetFirstIntrinsic(), container.GetSecondIntrinsic()));
		return container.GetVector3F();
	}
}
