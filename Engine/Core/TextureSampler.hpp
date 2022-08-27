// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Texture sampler class.
		 */
		class TextureSampler : public DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The sampler specification.
			 */
			explicit TextureSampler(const std::shared_ptr<Device>& pDevice, TextureSamplerSpecification&& specification) : DeviceBoundObject(pDevice), m_Specification(specification) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~TextureSampler() = default;

		protected:
			const TextureSamplerSpecification m_Specification = {};
		};
	}
}