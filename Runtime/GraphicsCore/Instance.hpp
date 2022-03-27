// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.hpp"

namespace Flint
{
	/**
	 * Flint instance backend object.
	 * This object is the main object for any Flint instance.
	 */
	template<class DeviceT, class DisplayT>
	class Instance : public FObject
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param enableValidation Whether or not to enable backend validation.
		 */
		Instance(bool enableValidation)
			: mEnableValidation(enableValidation)
		{
			std::filesystem::path workingDirectory = std::filesystem::current_path();
			if (!std::filesystem::exists(workingDirectory.string() + "\\Flint"))
				std::filesystem::create_directory(workingDirectory.string() + "\\Flint");

			std::filesystem::path flintRuntimeDirectory = workingDirectory.string() + "\\Flint";
			if (!std::filesystem::exists(flintRuntimeDirectory.string() + "\\Cache"))
				std::filesystem::create_directory(flintRuntimeDirectory.string() + "\\Cache");

			if (!std::filesystem::exists(flintRuntimeDirectory.string() + "\\Shaders"))
				std::filesystem::create_directory(flintRuntimeDirectory.string() + "\\Shaders");
		}

		/**
		 * Create a new device object.
		 *
		 * @param flags The device flags.
		 * @return The created device object.
		 */
		virtual std::unique_ptr<DeviceT> CreateDevice(const DeviceFlags flags) = 0;

		/**
		 * Create a new display object.
		 *
		 * @param extent The display extent.
		 * @param title The display title.
		 */
		virtual std::unique_ptr<DisplayT> CreateDisplay(const FBox2D& extent, const std::string& title) = 0;

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
}