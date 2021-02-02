// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanExporter.h"

#include "Core/Backend/Interface/ScreenBoundRenderTarget.h"
#include "Core/Backend/Interface/OffScreenRenderTarget.h"

#include "VulkanBackend/Buffers/VulkanStaggingBuffer.h"
#include "VulkanBackend/Buffers/VulkanVertexBuffer.h"
#include "VulkanBackend/Buffers/VulkanIndexBuffer.h"
#include "VulkanBackend/Buffers/VulkanUniformBuffer.h"

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

			Interface::CreateStaggingBuffer = CreateStaggingBuffer;
			Interface::MapStaggingBufferMemory = MapStaggingBufferMempory;
			Interface::UnmapStaggingBufferMemory = UnmapStaggingBufferMemory;
			Interface::DestroyStaggingBuffer = DestroyStaggingBuffer;

			Interface::CreateVertexBuffer = CreateVertexBuffer;
			Interface::MapVertexBufferMemory = MapVertexBufferMempory;
			Interface::UnmapVertexBufferMemory = UnmapVertexBufferMemory;
			Interface::DestroyVertexBuffer = DestroyVertexBuffer;

			Interface::CreateIndexBuffer = CreateIndexBuffer;
			Interface::MapIndexBufferMemory = MapIndexBufferMempory;
			Interface::UnmapIndexBufferMemory = UnmapIndexBufferMemory;
			Interface::DestroyIndexBuffer = DestroyIndexBuffer;

			Interface::CreateUniformBuffer = CreateUniformBuffer;
			Interface::MapUniformBufferMemory = MapUniformBufferMempory;
			Interface::UnmapUniformBufferMemory = UnmapUniformBufferMemory;
			Interface::DestroyUniformBuffer = DestroyUniformBuffer;
		}
	}
}