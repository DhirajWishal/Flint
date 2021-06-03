// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Instance.hpp"

#include <vulkan/vulkan.hpp>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanInstance final : public Instance
		{
		public:
			VulkanInstance(bool enableValidation);

			virtual Device& CreateDevice(DeviceFlags flags) override final;
			virtual void DestroyDevice(Device& device) override final;

			virtual void Terminate() override final;

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