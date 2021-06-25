// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "IndexBuffer.hpp"
#include "VulkanBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanIndexBuffer final : public IndexBuffer
		{
		public:
			VulkanIndexBuffer(Device& device, UI64 size, UI64 stride);

			virtual void CopyFromBuffer(const StaggingBuffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) override final;
			virtual void Terminate() override final;

		private:
			VulkanBuffer vBuffer;
		};
	}
}