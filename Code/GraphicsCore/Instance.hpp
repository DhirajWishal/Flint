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
		virtual std::shared_ptr<Device> CreateDevice(const DeviceFlags flags) = 0;

		/**
		 * Create a new display object.
		 *
		 * @param extent: The display extent.
		 * @param title: The display title.
		 */
		virtual std::shared_ptr<Display> CreateDisplay(const FBox2D& extent, const std::string& title) = 0;

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
		const bool IsValidationEnabled() const noexcept { return mEnableValidation; }

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
}