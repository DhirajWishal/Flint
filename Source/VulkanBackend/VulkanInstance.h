// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/FInstance.h"

#include <vulkan/vulkan.h> 

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanInstance final : public FInstance {
		public:
			VulkanInstance(bool enableValidation);

			virtual void Terminate() override final;

			virtual std::shared_ptr<FDevice> CreateDevice() override final;
			virtual std::shared_ptr<FDisplay> CreateDisplay(const Vector2 extent, const char* pTitle) override final;

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

		private:
			std::vector<const char*> mValidationLayers;

			VkInstance vInstance = VK_NULL_HANDLE;
			VkDebugUtilsMessengerEXT vDebugUtilsMessenger = VK_NULL_HANDLE;
		};
	}
}