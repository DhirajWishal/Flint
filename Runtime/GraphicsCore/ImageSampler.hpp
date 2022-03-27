// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Image.hpp"

namespace Flint
{
	/**
	 * Flint image sampler object.
	 * This object is used to sample images submitted to shaders.
	 */
	template<class DeviceT>
	class ImageSampler : public DeviceBoundObject<DeviceT>
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param specification The sampler specification.
		 */
		ImageSampler(DeviceT* pDevice, const ImageSamplerSpecification& specification) : DeviceBoundObject(pDevice), mSpecification(specification) {}

		/**
		 * Get the sampler specification.
		 *
		 * @return The specification.
		 */
		const ImageSamplerSpecification GetSpecification() const { return mSpecification; }

	protected:
		ImageSamplerSpecification mSpecification = {};
	};
}