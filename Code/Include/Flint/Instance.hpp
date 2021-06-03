// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.hpp"

namespace Flint
{
	class Device;

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

	DeviceFlags operator|(const DeviceFlags& lhs, const DeviceFlags& rhs) { return DeviceFlags{ static_cast<UI8>(lhs) | static_cast<UI8>(rhs) }; }
	DeviceFlags operator&(const DeviceFlags& lhs, const DeviceFlags& rhs) { return DeviceFlags{ static_cast<UI8>(lhs) & static_cast<UI8>(rhs) }; }

	/**
	 * Flint instance object.
	 * This object is the main object for any Flint instance.
	 */
	class FLINT_API Instance : public FObject
	{
	public:
		Instance(bool enableValidation) noexcept : mEnableValidation(enableValidation) {}

		/**
		 * Create a new device object.
		 *
		 * @param flags: The device flags.
		 * @return The created device object.
		 */
		virtual Device& CreateDevice(DeviceFlags flags) = 0;

		/**
		 * Destroy a created device.
		 *
		 * @param device: The device object.
		 */
		virtual void DestroyDevice(Device& device) = 0;

	public:
		/**
		 * Check if validation is enabled.
		 *
		 * @return Boolean value stating true or false.
		 */
		bool IsValidationEnabled() const noexcept { return mEnableValidation; }

	protected:
		bool mEnableValidation = true;
	};

	/**
	 * Create a new instance object.
	 *
	 * @param enableValidation: Whether or not to enable validation. We recommend using validation only in debug stage as it uses more resources.
	 * @return The instance object reference.
	 */
	FLINT_API Instance& CreateInstance(bool enableValidation);

	/**
	 * Destroy a created instance.
	 *
	 * @param instance: The instance to destroy.
	 */
	FLINT_API void DestroyInstance(Instance& instance);
}