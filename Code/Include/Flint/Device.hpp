// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	class Display;
	class DeviceBoundObject;
	class Instance;

	/**
	 * Device flags enum.
	 * This determines the device characteristics.
	 */
	enum class DeviceFlags : UI8 {
		/**
		 * This flag states to use external device (GPU) if available over integrated.
		 */
		EXTERNAL = BIT_SHIFT(0),

		/**
		 * This flag states to use only the integrated device if available. If an integrated one doesn't exist but an external one exists, it will automatically select the external device.
		 */
		INTEGRATED = BIT_SHIFT(1),

		/**
		 * State that the device is used for graphics. This is a must if graphics needs to be enabled.
		 */
		GRAPHICS_COMPATIBLE = BIT_SHIFT(2),

		/**
		 * This states the device is used for compute.
		 */
		COMPUTE_COMPATIBLE = BIT_SHIFT(3)
	};

	constexpr DeviceFlags operator|(const DeviceFlags& lhs, const DeviceFlags& rhs) { return DeviceFlags(static_cast<UI8>(lhs) | static_cast<UI8>(rhs)); }
	constexpr DeviceFlags operator&(const DeviceFlags& lhs, const DeviceFlags& rhs) { return DeviceFlags(static_cast<UI8>(lhs) & static_cast<UI8>(rhs)); }

	/**
	 * Flint device object.
	 * This object is the basis for all the resources. Almost all the resources are bound to a device.
	 */
	class FLINT_API Device : public FObject
	{
		friend Instance;

	public:
		Device(Instance& instance, DeviceFlags flags) noexcept : mInstance(instance), mFlags(flags) {}

		/**
		 * Check if the display is compatible with the device.
		 * 
		 * @param display: The display to check.
		 * @return Boolean value stating if compatible or not.
		 */
		virtual bool IsDisplayCompatible(const Display& display) = 0;

	protected:
		/**
		 * Terminate the device object.
		 */
		virtual void Terminate() = 0;

		/**
		 * Terminate a device bound object.
		 * 
		 * @param object: The object to terminate.
		 */
		void TerminateDeviceBoundObject(DeviceBoundObject& object) const { object.Terminate(); }

	protected:
		Instance& mInstance;
		DeviceFlags mFlags = DeviceFlags::EXTERNAL | DeviceFlags::GRAPHICS_COMPATIBLE;
	};
}