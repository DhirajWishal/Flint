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
		template<class TDevice>
		class DeviceBoundObject : public FObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device reference.
			 */
			explicit DeviceBoundObject(TDevice& device) : m_Device(device) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~DeviceBoundObject() = default;

			/**
			 * Get the device to which this object is bound to.
			 *
			 * @return The device reference.
			 */
			[[nodiscard]] TDevice& getDevice() { return m_Device; }

			/**
			 * Get the device to which this object is bound to.
			 *
			 * @return The const device reference.
			 */
			[[nodiscard]] const TDevice& getDevice() const { return m_Device; }

		private:
			TDevice& m_Device;
		};
	}
}