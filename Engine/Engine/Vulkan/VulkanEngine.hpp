// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Engine.hpp"

#include "VulkanBackend/VulkanInstance.hpp"
#include "VulkanBackend/VulkanDevice.hpp"

namespace Flint
{
	/**
	 * Vulkan engine class.
	 * This is the Vulkan backend engine.
	 */
	class VulkanEngine final : public std::enable_shared_from_this<VulkanEngine>, public Engine
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param applicationName The name of the application.
		 * @param applicationVersion The version of the application.
		 * @param enableBackendValidation Whether or not to enable backend validation. Turn this to true ONLY in the debug build.
		 */
		explicit VulkanEngine(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation);

		/**
		 * Destructor.
		 */
		~VulkanEngine() override;

		/**
		 * Destroy the engine.
		 */
		void destroy() override;

	private:
		VulkanBackend::VulkanInstance m_Instance;
		VulkanBackend::VulkanDevice m_Device;
	};
}