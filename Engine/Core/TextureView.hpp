// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Texture.hpp"

namespace Flint
{
	/**
	 * Texture view class.
	 * Views can be considered as a way to look at texture memory.
	 */
	class TextureView : public DeviceBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param pTexture The texture pointer.
		 */
		explicit TextureView(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Texture>& pTexture);

		/**
		 * Default virtual destructor.
		 */
		virtual ~TextureView() = default;

	protected:
		std::shared_ptr<Texture> m_pTexture = nullptr;
	};
}