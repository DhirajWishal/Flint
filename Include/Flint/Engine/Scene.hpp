// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Scene class.
	 * This contains all the instantiated entities and other objects.
	 */
	class Scene final : public DeviceBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		explicit Scene(const std::shared_ptr<Device>& pDevice) : DeviceBoundObject(pDevice) {}
	};
}