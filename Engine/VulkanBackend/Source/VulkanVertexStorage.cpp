// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanVertexStorage.hpp"
#include "VulkanBackend/VulkanCommandBuffers.hpp"

#include <Optick.h>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanVertexStorage::VulkanVertexStorage(const std::shared_ptr<VulkanDevice>& pDevice)
			: VertexStorage(pDevice)
		{
			validate();
		}

		VulkanVertexStorage::~VulkanVertexStorage()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanVertexStorage::terminate()
		{
			OPTICK_EVENT();

			// Terminate all the buffers.
			for (const auto& pBuffer : m_pBuffers)
			{
				if (pBuffer)
					pBuffer->terminate();
			}

			invalidate();
		}

		uint64_t VulkanVertexStorage::insert(VertexAttribute attribute, const Buffer* pStaggingBuffer)
		{
			OPTICK_EVENT();

			uint64_t offset = 0;

			// If a buffer already exists, we can move the content from the old buffer to a new one. If not let's create a new one.
			if (m_pBuffers[EnumToInt(attribute)])
			{
				const auto pOldBuffer = m_pBuffers[EnumToInt(attribute)];
				offset = pOldBuffer->getSize();

				auto pNewBuffer = std::static_pointer_cast<VulkanBuffer>(getDevice().createBuffer(offset + pStaggingBuffer->getSize(), BufferUsage::Vertex));

				// Create the command buffer to copy.
				auto vCommandBuffer = VulkanCommandBuffers(getDevicePointerAs<VulkanDevice>());
				vCommandBuffer.begin();

				// Copy the buffers.
				pNewBuffer->copyFromBatched(&vCommandBuffer, pOldBuffer.get(), 0, 0);
				pNewBuffer->copyFromBatched(&vCommandBuffer, pStaggingBuffer, 0, offset);

				// Submit the command.
				vCommandBuffer.end();
				vCommandBuffer.submitTransfer();
				vCommandBuffer.finishExecution();

				m_pBuffers[EnumToInt(attribute)] = pNewBuffer;
				pOldBuffer->terminate();
			}
			else
			{
				auto pNewBuffer = std::static_pointer_cast<VulkanBuffer>(getDevice().createBuffer(pStaggingBuffer->getSize(), BufferUsage::Vertex));
				pNewBuffer->copyFrom(pStaggingBuffer, 0, 0);

				m_pBuffers[EnumToInt(attribute)] = pNewBuffer;
			}

			return offset;
		}
	}
}