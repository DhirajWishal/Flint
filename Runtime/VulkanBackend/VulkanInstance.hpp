// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Instance.hpp"
#include "VulkanMacros.hpp"

#include <vector>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDisplay;

		class VulkanInstance final : public Instance<VulkanDevice, VulkanDisplay>
		{
		public:
			VulkanInstance(bool enableValidation, bool volkInitialized = false);
			~VulkanInstance() { if (!bIsTerminated) Terminate(); }

			virtual std::unique_ptr<VulkanDevice> CreateDevice(const DeviceFlags flags) override;
			virtual std::unique_ptr<VulkanDisplay> CreateDisplay(const FBox2D& extent, const std::string& title) override;

			virtual void Terminate() override;

		public:
			VkInstance GetInstance() const noexcept { return vInstance; }
			std::vector<const char*> GetValidationLayers() const noexcept { return mValidationLayers; }

		private:
			void InitializeGLFW();
			void TerminateGLFW();

			void InitializeInstance();
			void TerminateInstance();

			void InitializeDebugger();
			void TerminateDebugger();

		private:
			std::vector<const char*> mValidationLayers;

			VkInstance vInstance = VK_NULL_HANDLE;
			VkDebugUtilsMessengerEXT vDebugMessenger = VK_NULL_HANDLE;
		};
	}
}