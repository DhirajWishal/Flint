// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.hpp"
#include "Types.hpp"

#include <cstdint>
#include <string>
#include <memory>

namespace Flint
{
	namespace Core
	{
		class Device;

		/**
		 * Backend API enum.
		 */
		enum class BackendAPI : uint8_t
		{
			Vulkan = 1 << 0,
			DirectX_12 = 1 << 1
		};

		FLINT_DEFINE_ENUM_AND_OR(BackendAPI);

		/**
		 * Instance class.
		 * Every application requires just one instance of this class. It contains all the essential information, like validation and other information required by graphics engines.
		 *
		 * Validation requires a lot of resources and will hog up performance in release builds. Because of this, it is not recommended to be used in those types of builds. If in the development stage,
		 * we highly recommend using it.
		 *
		 * Note that in order to release all the resources in the instance, it needs to be destructed at the very end, after destroying all the other objects.
		 */
		class Instance : public FObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param applicationName The name of the application.
			 * @param applicationVersion The application version.
			 * @param api The backend API type.
			 * @param enableValidation Whether or not to enable backend validation.
			 */
			explicit Instance(std::string&& applicationName, uint32_t applicationVersion, BackendAPI api, bool enableValidation)
				: m_ApplicationName(std::move(applicationName)), m_ApplicationVersion(applicationVersion), m_BackendAPI(api), m_EnableValidation(enableValidation) {}

			/**
			 * Virtual destructor.
			 */
			virtual ~Instance() = default;

			/**
			 * Create a new device.
			 *
			 * @return The device pointer.
			 */
			[[nodiscard]] virtual std::shared_ptr<Device> createDevice() = 0;

			/**
			 * Get the backend API.
			 *
			 * @return The backend API type.
			 */
			[[nodiscard]] BackendAPI getBackendAPI() const { return m_BackendAPI; }

			/**
			 * Check if the backend validation is enabled.
			 *
			 * @return Whether or not backend validation enabled.
			 */
			[[nodiscard]] bool isValidationEnabled() const { return m_EnableValidation; }

			/**
			 * Get the name of the application.
			 *
			 * @return The application name.
			 */
			[[nodiscard]] std::string_view getApplicationName() const { return m_ApplicationName; }

			/**
			 * Get the application version.
			 *
			 * @return The application version.
			 */
			[[nodiscard]] uint32_t getApplicationVersion() const { return m_ApplicationVersion; }

		private:
			const std::string m_ApplicationName;
			const uint32_t m_ApplicationVersion;
			const BackendAPI m_BackendAPI;
			const bool m_EnableValidation;
		};
	}
}