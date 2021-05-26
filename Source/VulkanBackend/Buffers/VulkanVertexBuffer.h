// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/VertexBuffer.h"
#include "VulkanBackend/VulkanBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanVertexBuffer final : public Backend::VertexBuffer, public VulkanBuffer
		{
		public:
			VulkanVertexBuffer(VulkanDevice* pDevice, UI64 size, Backend::MemoryProfile memoryProfile);

			virtual void Terminate() override final;
			virtual void* MapMemory(UI64 size, UI64 offset) override final;
			virtual void UnmapMemory() override final;
			virtual void FlushMemoryMappings() override final;

			virtual void CopyFrom(const Backend::Buffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) override final;
		};
	}
}