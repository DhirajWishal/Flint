// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBufferManager.h"
#include "VulkanBackend/VulkanCommandBuffer.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/RenderTargets/VulkanScreenBoundRenderTargetS.h"

#include "Core/Backend/Pipeline.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanCommandBufferManager::CreateBuffers(Backend::Device* pDevice, UI32 count)
		{
			this->pDevice = pDevice;
			this->mBufferCount = count;
			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();

			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = pvDevice->GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(pvDevice->CreateCommanPool(&vPoolCI, &vCommandPool), "Failed to create command pool!")

				VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandBufferCount = count;
			vAI.commandPool = vCommandPool;

			vCommandBuffers.resize(count);
			FLINT_VK_ASSERT(pvDevice->AllocateCommandBuffers(&vAI, vCommandBuffers), "Failed to allocate command buffers!")

				mCommandBuffers.reserve(vCommandBuffers.size());
			for (VkCommandBuffer buffer : vCommandBuffers)
				INSERT_INTO_VECTOR(mCommandBuffers, std::make_shared<VulkanCommandBuffer>(buffer));
		}

		void VulkanCommandBufferManager::Terminate()
		{
			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			//pvDevice->FreeComandBuffers(vCommandPool, vCommandBuffers);
			pvDevice->DestroyCommandPool(vCommandPool);

			vCommandBuffers.clear();
			mCommandBuffers.clear();
		}

		std::shared_ptr<Backend::CommandBufferManager> VulkanCommandBufferManager::CreateChild(UI32 bufferCount)
		{
			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();

			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = pvDevice->GetQueue().mTransferFamily.value();

			VkCommandPool vPool = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(pvDevice->CreateCommanPool(&vPoolCI, &vPool), "Failed to create command pool!")

				VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			vAI.commandBufferCount = bufferCount;
			vAI.commandPool = vPool;

			std::vector<VkCommandBuffer> vCommandBuffers(bufferCount);
			FLINT_VK_ASSERT(pvDevice->AllocateCommandBuffers(&vAI, vCommandBuffers), "Failed to allocate command buffers!")

			std::vector<std::shared_ptr<Backend::CommandBuffer>> commandBuffers;
			for (VkCommandBuffer buffer : vCommandBuffers)
				INSERT_INTO_VECTOR(commandBuffers, std::make_shared<VulkanCommandBuffer>(buffer));

			return std::make_shared<VulkanCommandBufferManager>(pDevice, vPool, std::move(vCommandBuffers), std::move(commandBuffers));
		}

		void VulkanCommandBufferManager::UpdateChild(Backend::CommandBufferManager* pChildBufferManager, Backend::RenderTarget* pRenderTarget)
		{
			vCBII.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			vCBII.pNext = VK_NULL_HANDLE;
			vCBII.occlusionQueryEnable = VK_FALSE;
			vCBII.subpass = 0;
			vCBII.renderPass = pRenderTarget->Derive<VulkanScreenBoundRenderTargetS>()->GetRenderPass();
			vCBII.framebuffer = pRenderTarget->Derive<VulkanScreenBoundRenderTargetS>()->GetCurrentFrameBuffer();

			for (auto& buffer : pChildBufferManager->GetBuffers())
				buffer->Derive<VulkanCommandBuffer>()->SetInheritanceInfo(&vCBII);
		}

		void VulkanCommandBufferManager::ClearBuffers()
		{
			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			pvDevice->FreeComandBuffers(vCommandPool, vCommandBuffers);
			
			vCommandBuffers.clear();
			mCommandBuffers.clear();
		}

		void VulkanCommandBufferManager::RecreateBuffers()
		{
			VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandBufferCount = mBufferCount;
			vAI.commandPool = vCommandPool;

			vCommandBuffers.resize(mBufferCount);
			FLINT_VK_ASSERT(pDevice->Derive<VulkanDevice>()->AllocateCommandBuffers(&vAI, vCommandBuffers), "Failed to allocate command buffers!")

				mCommandBuffers.clear();
			mCommandBuffers.reserve(vCommandBuffers.size());
			for (VkCommandBuffer buffer : vCommandBuffers)
				INSERT_INTO_VECTOR(mCommandBuffers, std::make_shared<VulkanCommandBuffer>(buffer));
		}
	}
}