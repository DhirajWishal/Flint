// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/FObject.hpp"
#include "Core/Error.hpp"
#include "Core/Hasher.hpp"
#include "GraphicsTypes.h"

#include <vector>

namespace Flint
{
	/**
	 * Flint device bound object.
	 * This object is the base class for all the resources in Flint which are based on a device.
	 */
	template<class DeviceT>
	class DeviceBoundObject : public FObject
	{
		friend DeviceT;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		explicit DeviceBoundObject(DeviceT* pDevice) : m_pDevice(pDevice)
		{
			if (!pDevice)
				throw std::invalid_argument("Device pointer should not be null!");
		}

		/**
		 * Get the device of this object.
		 *
		 * @return The device pointer.
		 */
		DeviceT* GetDevice() const { return m_pDevice; }

		/**
		 * Terminate the device bound object.
		 */
		virtual void Terminate() = 0;

	protected:
		DeviceT* m_pDevice = nullptr;
	};
}