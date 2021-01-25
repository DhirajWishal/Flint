// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanExporter.h"

#include "VulkanBackend/RenderTargets/VulkanRenderTargetSB2D.h"
#include "VulkanBackend/RenderTargets/VulkanRenderTargetSB3D.h"

#include "Core/ErrorHandler/Logger.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void ExportFunctions()
		{
			// Initialize the Vulkan Meta Loader.
			if (volkInitialize() != VK_SUCCESS)
				Logger::LogError(TEXT("Failed to initialize the Vulkan Meta Loader!"));

			Interface::CreateInstance = CreateInstance;
			Interface::DestroyInstance = DestroyInstance;

			Interface::CreateDisplay = CreateDisplay;
			Interface::DestroyDisplay = DestroyDisplay;
			Interface::GetInputCenter = GetInputCenter;

			Interface::CreateDevice = CreateDevice;
			Interface::DestroyDevice = DestroyDevice;

			Interface::CreateRenderTargetSB2D = CreateRenderTargetSB2D;
			Interface::DestroyRenderTargetSB2D = DestroyRenderTargetSB2D;

			Interface::CreateRenderTargetSB3D = CreateRenderTargetSB3D;
			Interface::DestroyRenderTargetSB3D = DestroyRenderTargetSB3D;
		}
	}
}