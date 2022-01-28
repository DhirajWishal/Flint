// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.GraphicsCore.Instance;

import Flint.Core.FObject;
import Flint.GraphicsCore.Display;
import Flint.GraphicsCore.Device;
import <memory>;
import <string>;

export namespace Flint
{
	class Device;

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
		 * @param enableValidation Whether or not to enable backend validation.
		 */
		Instance(bool enableValidation);

		/**
		 * Create a new device object.
		 *
		 * @param flags The device flags.
		 * @return The created device object.
		 */
		virtual std::shared_ptr<Device> CreateDevice(const DeviceFlags flags) = 0;

		/**
		 * Create a new display object.
		 *
		 * @param extent The display extent.
		 * @param title The display title.
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
	 * @param enableValidation Whether or not to enable validation. We recommend using validation only in debug stage as it uses more resources.
	 * @return The instance object reference.
	 */
	std::shared_ptr<Instance> CreateInstance(bool enableValidation);
}

module: private;

import <filesystem>;

namespace Flint
{
	Instance::Instance(bool enableValidation)
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

	std::shared_ptr<Instance> CreateInstance(bool enableValidation)
	{
		return nullptr;
	}
}