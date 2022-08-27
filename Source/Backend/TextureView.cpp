// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Backend/TextureView.hpp"
#include "Flint/Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{
	namespace Backend
	{
		TextureView::TextureView(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Texture>& pTexture)
			: DeviceBoundObject(pDevice), m_pTexture(pTexture)
		{
			if (!pTexture)
				throw InvalidArgumentError("The texture pointer should not be null!");
		}
	}
}