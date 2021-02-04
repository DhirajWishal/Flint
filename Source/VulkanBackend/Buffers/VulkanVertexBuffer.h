// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanVertexBuffer final : public VulkanBuffer {
		public:
			VulkanVertexBuffer() {}
			~VulkanVertexBuffer() {}

			virtual void Initialize(VulkanDevice* pDevice, UI64 size) override final;
			virtual void Terminate() override final;
		};

		Interface::VertexBufferHandle CreateVertexBuffer(const Interface::DeviceHandle& deviceHandle, UI64 size);
		void* MapVertexBufferMempory(const Interface::VertexBufferHandle& handle, UI64 size, UI64 offset);
		void UnmapVertexBufferMemory(const Interface::VertexBufferHandle& handle);
		void DestroyVertexBuffer(const Interface::VertexBufferHandle& handle);
	}
}