// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanIndexBuffer final : public VulkanBuffer {
		public:
			VulkanIndexBuffer() {}
			~VulkanIndexBuffer() {}

			virtual void Initialize(VulkanDevice* pDevice, UI64 size) override final;
			virtual void Terminate() override final;
		};

		Interface::IndexBufferHandle CreateIndexBuffer(const Interface::DeviceHandle& deviceHandle, UI64 size);
		void* MapIndexBufferMempory(const Interface::IndexBufferHandle& handle, UI64 size, UI64 offset);
		void UnmapIndexBufferMemory(const Interface::IndexBufferHandle& handle);
		void DestroyIndexBuffer(const Interface::IndexBufferHandle& handle);
	}
}