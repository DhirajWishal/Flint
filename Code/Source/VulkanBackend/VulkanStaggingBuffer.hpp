// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/StaggingBuffer.hpp"
#include "VulkanBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanStaggingBuffer final : public StaggingBuffer
		{
		public:
			VulkanStaggingBuffer(Device& device, UI64 size);

			virtual void* MapMemory(UI64 size, UI64 offset = 0) override final;
			virtual void UnmapMemory() override final;
			virtual void Terminate() override final;

		public:
			VulkanBuffer vBuffer;
		};
	}
}