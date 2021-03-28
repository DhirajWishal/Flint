// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend\VulkanExporter.h"

#include "VulkanBackend\VulkanDevice.h"
#include "VulkanBackend\VulkanDisplay.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void ExportVulkanFunctions()
		{
			Backend::CreateInstance = CreateInstance;
			Backend::DestroyInstance = DestroyInstance;

			Backend::CreateDevice = CreateDevice;
			Backend::DestroyDevice = DestroyDevice;

			Backend::CreateDisplay = CreateDisplay;
			Backend::UpdateDisplay = UpdateDisplay;
			Backend::DestroyDisplay = DestroyDisplay;
			Backend::GetDisplayInputCenter = GetDisplayInputCenter;
		}
	}
}