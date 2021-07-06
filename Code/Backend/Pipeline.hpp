// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Flint pipeline object.
	 * This object is used to store information about a rendering or compute pipeline.
	 */
	class Pipeline : public DeviceBoundObject
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 */
		Pipeline(const std::shared_ptr<Device>& pDevice) : DeviceBoundObject(pDevice) {}
	};
}