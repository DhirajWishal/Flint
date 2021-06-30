// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "VulkanMacros.hpp"

#include <vector>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanInstance final : public Backend::Instance
		{
		public:
			VulkanInstance(bool enableValidation);

			virtual std::shared_ptr<Backend::Device> CreateDevice(Backend::DeviceFlags flags) override final;
			virtual void DestroyDevice(const std::shared_ptr<Backend::Device>& pDevice) override final;

			virtual std::shared_ptr<Backend::Display> CreateDisplay(const FExtent2D& extent, const std::string& title) override final;
			virtual void DestroyDisplay(const std::shared_ptr<Backend::Display>& pDisplay) override final;

			virtual void Terminate() override final;

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