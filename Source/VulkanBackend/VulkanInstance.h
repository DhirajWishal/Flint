// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Instance.h"

#include "MetaLoader/volk.h"  

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanInstance : public Backend::Instance {
		public:
			VulkanInstance() {}
			~VulkanInstance() {}

			virtual void Initialize(bool enableValidation) override final;
			virtual void Terminate() override final;

			virtual Backend::Display* CreateDisplay(UI32 width, UI32 height, const char* pTitle) override final;

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
		};
	}
}