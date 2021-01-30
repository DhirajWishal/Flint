// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanResourceContainer.h"
#include "VulkanBackend/VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanResourceContainer::Initialize(VulkanDevice* pDevice, const std::vector<VkDescriptorPoolSize>& sizes, VkDescriptorSetLayout vLayout)
		{
			this->pDevice = pDevice;

			VkDescriptorPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			vPoolCI.flags = VK_NULL_HANDLE;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.maxSets = 1;
			vPoolCI.poolSizeCount = static_cast<UI32>(sizes.size());
			vPoolCI.pPoolSizes = sizes.data();

			FLINT_VK_ASSERT(pDevice->CreateDescriptorPool(&vPoolCI, &vPool), "Failed to create descriptor pool!")

				VkDescriptorSetAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.descriptorPool = vPool;
			vAI.descriptorSetCount = 1;
			vAI.pSetLayouts = &vLayout;

			FLINT_VK_ASSERT(pDevice->AllocateDescriptorSet(&vAI, &vSet), "Failed to allocate descriptor set!")
		}

		void VulkanResourceContainer::Terminate()
		{
			pDevice->DestroyDescriptorPool(vPool);
		}
		
		void VulkanResourceContainer::RegisterBuffer(VulkanUniformBuffer* pUniformBuffer)
		{
			VkDescriptorBufferInfo vBI = {};
			vBI.buffer = pUniformBuffer->GetBuffer();
			vBI.offset = 0;
			vBI.range = VK_WHOLE_SIZE;

			VkWriteDescriptorSet vWS = {};
			vWS.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			vWS.pNext = VK_NULL_HANDLE;
		}
	}
}