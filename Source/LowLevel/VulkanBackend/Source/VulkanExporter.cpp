// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanExporter.h"
#include "VulkanBackend/VulkanInstance.h"
#include "VulkanBackend/VulkanDisplay.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void ExportFunctions()
		{
			Interface::CreateInstance = CreateInstance;
			Interface::DestroyInstance = DestroyInstance;

			Interface::CreateDisplay = CreateDisplay;
			Interface::DestroyDisplay = DestroyDisplay;
			Interface::GetInputCenter = GetInputCenter;
		}
	}
}