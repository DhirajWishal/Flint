// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "UniformBuffer.hpp"
#include "VulkanBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanUniformBuffer final : public UniformBuffer
		{
		public:
			VulkanUniformBuffer(Device& device, UI64 size);

			virtual void* MapMemory(UI64 size, UI64 offset = 0) override final;
			virtual void UnmapMemory() override final;
			virtual void Terminate() override final;

		private:
			VulkanBuffer vBuffer;
		};
	}
}