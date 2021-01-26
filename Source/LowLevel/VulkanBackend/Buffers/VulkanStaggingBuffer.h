// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanStaggingBuffer final : public VulkanBuffer {
		public:
			VulkanStaggingBuffer() {}
			~VulkanStaggingBuffer() {}

			virtual void Initialize(VulkanDevice* pDevice, UI64 size) override final;
			virtual void Terminate() override final;
		};

		Interface::StaggingBufferHandle CreateStaggingBuffer(const Interface::DeviceHandle& deviceHandle, UI64 size);
		void* MapBufferMempory(const Interface::StaggingBufferHandle& handle, UI64 size, UI64 offset);
		void UnmapBufferMemory(const Interface::StaggingBufferHandle& handle);
		void DestroyBuffer(const Interface::StaggingBufferHandle& handle);
	}
}