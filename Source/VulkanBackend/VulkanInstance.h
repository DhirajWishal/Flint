// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Instance.h"

#include <vulkan/vulkan.h> 

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanInstance {
		public:
			VulkanInstance() {}
			~VulkanInstance() {}

			void Initialize(bool enableValidation);
			void Terminate();

			//virtual Backend::Device* CreateDevice() override final;
		private:
			void InitializeGLFW();
			void TerminateGLFW();

			void CreateInstance();
			void DestroyInstance();

			void CreateDebugMessenger();
			void DestroyDebugMessenger();

		public:
			VkInstance GetInstance() const { return vInstance; }
			std::vector<const char*>& GetValidationLayers() { return mValidationLayers; }
			bool IsValidationEnabled() const { return mEnableValidation; }

		private:
			std::vector<const char*> mValidationLayers;

			VkInstance vInstance = VK_NULL_HANDLE;
			VkDebugUtilsMessengerEXT vDebugUtilsMessenger = VK_NULL_HANDLE;

			bool mEnableValidation = true;
		};

		Backend::InstanceHandle CreateInstance(bool enableValidation);
		void DestroyInstance(Backend::InstanceHandle handle);
	}
}