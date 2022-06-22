// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Device bound object class.
		 * This class is the base class for all the device bound objects.
		 */
		class DeviceBoundObject : public FObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device reference.
			 */
			explicit DeviceBoundObject(const std::shared_ptr<Device>& pDevice) : m_pDevice(pDevice) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~DeviceBoundObject() = default;

			/**
			 * Get the device to which this object is bound to.
			 *
			 * @return The device reference.
			 */
			[[nodiscard]] Device& getDevice() { return *m_pDevice; }

			/**
			 * Get the device to which this object is bound to.
			 *
			 * @return The const device reference.
			 */
			[[nodiscard]] const Device& getDevice() const { return *m_pDevice; }

		private:
			std::shared_ptr<Device> m_pDevice = nullptr;
		};
	}
}