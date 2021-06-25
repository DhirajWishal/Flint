// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VertexBuffer.hpp"
#include "VulkanBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanVertexBuffer final : public VertexBuffer
		{
		public:
			VulkanVertexBuffer(Device& device, UI64 size, const VertexDescriptor& descriptor);

			virtual void CopyFromBuffer(const StaggingBuffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) override final;
			virtual void Terminate() override final;

		private:
			VulkanBuffer vBuffer;
		};
	}
}