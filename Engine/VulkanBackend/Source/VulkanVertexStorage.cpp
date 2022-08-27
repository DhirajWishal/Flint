// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanVertexStorage.hpp"
#include "VulkanBackend/VulkanCommandBuffers.hpp"

#include <Optick.h>

#include <atomic>

namespace Flint
{
	namespace Backend
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
			for (auto& pBuffer : m_pBuffers)
			{
				pBuffer.apply([](std::shared_ptr<VulkanBuffer>& pVertexBuffer)
					{
						if (pVertexBuffer)
							pVertexBuffer->terminate();
					}
				);
			}

			invalidate();
		}

		uint64_t VulkanVertexStorage::insert(VertexAttribute attribute, const Buffer* pStaggingBuffer)
		{
			OPTICK_EVENT();

			// Skip if we don't have anything to copy.
			if (!pStaggingBuffer)
				return 0;

			return m_pBuffers[EnumToInt(attribute)].apply([this, pStaggingBuffer](std::shared_ptr<VulkanBuffer>& pOldBuffer)
				{
					uint64_t offset = 0;

					// If a buffer already exists, we can move the content from the old buffer to a new one. If not let's create a new one.
					if (pOldBuffer)
					{
						offset = pOldBuffer->getSize();

						auto pNewBuffer = std::static_pointer_cast<VulkanBuffer>(getDevice().createBuffer(offset + pStaggingBuffer->getSize(), BufferUsage::Vertex));

						// Create the command buffer to copy.
						auto vCommandBuffer = VulkanCommandBuffers(getDevicePointerAs<VulkanDevice>());
						vCommandBuffer.begin();

						// Copy the buffers.
						pNewBuffer->copyFromBatched(&vCommandBuffer, pOldBuffer.get());
						pNewBuffer->copyFromBatched(&vCommandBuffer, pStaggingBuffer, 0, offset);

						// Submit the command.
						vCommandBuffer.end();
						vCommandBuffer.submitTransfer();
						vCommandBuffer.finishExecution();

						pOldBuffer = std::move(pNewBuffer);
					}
					else
					{
						pOldBuffer = std::static_pointer_cast<VulkanBuffer>(getDevice().createBuffer(pStaggingBuffer->getSize(), BufferUsage::Vertex));
						pOldBuffer->copyFrom(pStaggingBuffer);
					}

					return offset;
				}
			);
		}
	}
}