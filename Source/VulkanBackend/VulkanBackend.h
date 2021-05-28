// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/FBackend.h"

#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanDisplay.h"

#include <unordered_map>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBackend final : public FBackend
		{
		public:
			VulkanBackend(bool enableValidation);

			virtual void Terminate() override final;

			/** Device functions. */
		public:
			virtual FDeviceHandle CreateDevice() override final;
			virtual void Terminate(FDeviceHandle& handle) override final;

			/** Display functions. */
		public:
			virtual FDisplayHandle CreateDisplay(const UI32 width, const UI32 height, const std::string& title) override final;
			virtual void UpdateDisplay(const FDisplayHandle& handle) override final;
			virtual void Terminate(FDisplayHandle& handle) override final;

		private:
			VulkanInstance mInstance = {};

			std::unordered_map<UI8, VulkanDevice> mDevices;
			UI8 mDeviceIndex = 0;

			std::unordered_map<UI8, VulkanDisplay> mDisplays;
			UI8 mDisplayIndex = 0;
		};
	}
}