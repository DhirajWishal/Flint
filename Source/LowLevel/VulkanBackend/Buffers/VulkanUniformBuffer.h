// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanUniformBuffer final : public VulkanBuffer {
		public:
			VulkanUniformBuffer() {}
			~VulkanUniformBuffer() {}

			virtual void Initialize(VulkanDevice* pDevice, UI64 size) override final;
			virtual void Terminate() override final;
		};

		Interface::UniformBufferHandle CreateUniformBuffer(const Interface::DeviceHandle& deviceHandle, UI64 size);
		void* MapUniformBufferMempory(const Interface::UniformBufferHandle& handle, UI64 size, UI64 offset);
		void UnmapUniformBufferMemory(const Interface::UniformBufferHandle& handle);
		void DestroyUniformBuffer(const Interface::UniformBufferHandle& handle);
	}
}