// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core\Backend\Image.h"

namespace Flint
{
	namespace Backend
	{
		const bool SamplerSpecification::operator==(const SamplerSpecification& other) const
		{
			return
				other.mMipLevel == this->mMipLevel &&
				other.mMinLOD == this->mMinLOD &&
				other.mLODBias == this->mLODBias &&
				other.mFilter == this->mFilter &&
				other.mBorderColor == this->mBorderColor &&
				other.mAddressModeU == this->mAddressModeU &&
				other.mAddressModeV == this->mAddressModeV &&
				other.mAddressModeW == this->mAddressModeW;
		}
	}
}