// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend\VulkanCommandBufferList.h"
#include "VulkanBackend\VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanCommandBufferList::Initialize(DeviceType* pDevice, UI64 bufferCount)
		{
			this->pDevice = pDevice;
			this->mBufferCount = bufferCount;

			VkCommandPoolCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.flags = VK_NULL_HANDLE;
			vCI.queueFamilyIndex = pDevice->GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(pDevice->CreateCommanPool(&vCI, &vCommandPool), "Failed to create command pool!")

				VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandPool = vCommandPool;
			vAI.commandBufferCount = static_cast<UI32>(GetBufferCount());

			FLINT_VK_ASSERT(pDevice->AllocateCommandBuffers(&vAI, vBuffers), "Failed to allocate command buffer!")
		}

		void VulkanCommandBufferList::Terminate()
		{
			pDevice->FreeComandBuffers(vCommandPool, vBuffers);
			pDevice->DestroyCommandPool(vCommandPool);
		}

		void VulkanCommandBufferList::BeginBuffer(UI64 index)
		{
			VkCommandBufferBeginInfo vBI = {};
			vBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBI.pNext = VK_NULL_HANDLE;
			vBI.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBI.pInheritanceInfo = VK_NULL_HANDLE;

			FLINT_VK_ASSERT(vkBeginCommandBuffer(vBuffers[GetIndex()], &vBI), "Failed to begin command buffer recording!")
		}

		void VulkanCommandBufferList::EndBuffer()
		{
			FLINT_VK_ASSERT(vkEndCommandBuffer(vBuffers[GetIndex()]), "Failed to end command buffer recording!")
		}

		void VulkanCommandBufferList::BindVertexBuffer(const BufferType& buffer, UI64 firstBinding, UI64 bindingCount)
		{
#ifdef FLINT_DEBUG
			if (buffer.GetUsage() != Backend::BufferUsage::VERTEX)
			{
				FLINT_LOG_ERROR(TEXT("Buffer type is not vertex!"))
					return;
			}
#endif // DEBUG

			VkDeviceSize pOffset[1] = { 0 };
			vkCmdBindVertexBuffers(GetCurrentBuffer(), static_cast<UI32>(firstBinding), static_cast<UI32>(bindingCount), &buffer.vBuffer, pOffset);
		}
	}
}
