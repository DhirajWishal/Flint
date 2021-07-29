// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.hpp"

namespace Flint
{
	/**
	 * Flint instance backend object.
	 * This object is the main object for any Flint instance.
	 */
	class Instance : public FObject
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param enableValidation: Whether or not to enable backend validation.
		 */
		Instance(bool enableValidation);

		/**
		 * Create a new device object.
		 *
		 * @param flags: The device flags.
		 * @return The created device object.
		 */
		virtual std::shared_ptr<Device> CreateDevice(DeviceFlags flags) = 0;

		/**
		 * Destroy a created device.
		 *
		 * @param pDevice: The device object.
		 */
		virtual void DestroyDevice(const std::shared_ptr<Device>& pDevice) = 0;

		/**
		 * Create a new display object.
		 *
		 * @param extent: The display extent.
		 * @param title: The display title.
		 */
		virtual std::shared_ptr<Display> CreateDisplay(const FExtent2D& extent, const std::string& title) = 0;

		/**
		 * Destroy a created display.
		 *
		 * @param pDisplay: The display to destroy.
		 */
		virtual void DestroyDisplay(const std::shared_ptr<Display>& pDisplay) = 0;

		/**
		 * Terminate the instance object.
		 */
		virtual void Terminate() = 0;

	public:
		/**
		 * Check if validation is enabled.
		 *
		 * @return Boolean value stating true or false.
		 */
		bool IsValidationEnabled() const noexcept { return mEnableValidation; }

	protected:
		/**
		 * Terminate a device object.
		 *
		 * @param device: The device to terminate.
		 */
		void TerminateDevice(const std::shared_ptr<Device>& pDevice) const { pDevice->Terminate(); }

		/**
		 * Terminate a display object.
		 *
		 * @param display: The display to terminate.
		 */
		void TerminateDisplay(const std::shared_ptr<Display>& pDisplay) const { pDisplay->Terminate(); }

	protected:
		bool mEnableValidation = true;
	};

	/**
	 * Create a new instance object.
	 *
	 * @param enableValidation: Whether or not to enable validation. We recommend using validation only in debug stage as it uses more resources.
	 * @return The instance object reference.
	 */
	std::shared_ptr<Instance> CreateInstance(bool enableValidation);

	/**
	 * Destroy a created instance.
	 *
	 * @param instance: The instance to destroy.
	 */
	void DestroyInstance(const std::shared_ptr<Instance>& pInstance);
}