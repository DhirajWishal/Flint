// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanOneTimeCommandBuffer.h"
#include "VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanOneTimeCommandBuffer::VulkanOneTimeCommandBuffer(const VulkanDevice* pDevice) : pDevice(const_cast<VulkanDevice*>(pDevice))
		{
			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = VK_NULL_HANDLE;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = this->pDevice->GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(pDevice->CreateCommanPool(&vPoolCI, &vPool), "Failed to create command pool!");

			VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandPool = vPool;
			vAI.commandBufferCount = 1;

			{
				std::vector<VkCommandBuffer> vCommandBuffers(1);
				FLINT_VK_ASSERT(pDevice->AllocateCommandBuffers(&vAI, vCommandBuffers), "Failed to allocate command buffers!");
				vBuffer = vCommandBuffers[0];
			}

			VkCommandBufferBeginInfo vBI = {};
			vBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			//vBI.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			FLINT_VK_ASSERT(pDevice->BeginCommandBuffer(vBuffer, &vBI), "Failed to begin command buffer recording!");
		}

		VulkanOneTimeCommandBuffer::~VulkanOneTimeCommandBuffer()
		{
			FLINT_VK_ASSERT(pDevice->EndCommandBuffer(vBuffer), "Failed to end command buffer recording!");

			VkSubmitInfo vSI = {};
			vSI.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSI.commandBufferCount = 1;
			vSI.pCommandBuffers = &vBuffer;

			VkFenceCreateInfo vFCI = {};
			vFCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vFCI.pNext = VK_NULL_HANDLE;
			vFCI.flags = VK_NULL_HANDLE;

			std::vector<VkFence> vFences{ 1 };
			FLINT_VK_ASSERT(pDevice->CreateFences(&vFCI, vFences), "Failed to create fence!");

			FLINT_VK_ASSERT(pDevice->SubmitQueue(pDevice->GetTransferQueue(), { vSI }, vFences[0]), "Failed to submit queue!");
			FLINT_VK_ASSERT(pDevice->WaitForFences(vFences, true, std::numeric_limits<UI32>::max()), "Failed to wait for fences!");
			pDevice->DestroyFences(vFences);

			pDevice->FreeComandBuffers(vPool, { vBuffer });
			pDevice->DestroyCommandPool(vPool);
		}
	}
}