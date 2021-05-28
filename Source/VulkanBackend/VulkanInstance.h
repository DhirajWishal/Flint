// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Templates/Instance.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanInstance final : public Templates::Instance
		{
		public:
			VulkanInstance() = default;

			void InitializeGLFW();
			void TerminateGLFW();

			void CreateInstance(bool enableValidation);
			void DestroyInstance();

			void CreateDebugMessenger();
			void DestroyDebugMessenger();

		public:
			bool IsValidationEnabled() const { return vDebugUtilsMessenger != VK_NULL_HANDLE; }

		public:
			VkInstance GetInstance() const { return vInstance; }
			std::vector<const char*>& GetValidationLayers() { return mValidationLayers; }

		private:
			std::vector<const char*> mValidationLayers;

			VkInstance vInstance = VK_NULL_HANDLE;
			VkDebugUtilsMessengerEXT vDebugUtilsMessenger = VK_NULL_HANDLE;
		};
	}
}