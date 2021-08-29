// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Instance.hpp"
#include "VulkanMacros.hpp"

#include <vector>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanInstance final : public Instance, public boost::enable_shared_from_this<VulkanInstance>
		{
		public:
			VulkanInstance(bool enableValidation);
			~VulkanInstance() { if (!bIsTerminated) Terminate(); }

			virtual boost::shared_ptr<Device> CreateDevice(const DeviceFlags flags) override final;
			virtual boost::shared_ptr<Display> CreateDisplay(const FBox2D& extent, const std::string& title) override final;

			virtual void Terminate() override final;

		public:
			VkInstance GetInstance() const noexcept { return vInstance; }
			boost::container::vector<const char*> GetValidationLayers() const noexcept { return mValidationLayers; }

		private:
			void InitializeGLFW();
			void TerminateGLFW();

			void InitializeInstance();
			void TerminateInstance();

			void InitializeDebugger();
			void TerminateDebugger();

		private:
			boost::container::vector<const char*> mValidationLayers;

			VkInstance vInstance = VK_NULL_HANDLE;
			VkDebugUtilsMessengerEXT vDebugMessenger = VK_NULL_HANDLE;
		};
	}
}