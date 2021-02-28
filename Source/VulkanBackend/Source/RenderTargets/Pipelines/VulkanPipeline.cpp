// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/Pipelines/VulkanPipeline.h"
#include "VulkanBackend/RenderTargets/Pipelines/VulkanGraphicsPipeline.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/VulkanBuffer.h"
#include "VulkanBackend/VulkanUtilities.h"
#include "VulkanBackend/VulkanCommandBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanPipelineResource::Initialize(Backend::Pipeline* pPipeline)
		{
			this->pPipeline = pPipeline;
			VulkanPipeline* pvPipeline = pPipeline->Derive<VulkanGraphicsPipeline>()->Derive<VulkanPipeline>();
			CreateDescriptorPool(pvPipeline->vPoolSizes);

			VkDescriptorSetAllocateInfo vDSAI = {};
			vDSAI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			vDSAI.pNext = VK_NULL_HANDLE;
			vDSAI.descriptorSetCount = 1;
			vDSAI.pSetLayouts = &pvPipeline->vSetLayout;
			vDSAI.descriptorPool = vPool;

			FLINT_VK_ASSERT(pPipeline->GetRenderTarget()->GetDevice()->Derive<VulkanDevice>()->AllocateDescriptorSet(&vDSAI, &vDescriptorSet), "Failed to allocate descriptor set!");
		}

		void VulkanPipelineResource::Terminate()
		{
			VulkanDevice* pDevice = pPipeline->GetRenderTarget()->GetDevice()->Derive<VulkanDevice>();
			pDevice->DestroyDescriptorPool(vPool);
		}

		void VulkanPipelineResource::RegisterUniformBuffers(const Backend::UniformBufferContainer& uniformBuffers)
		{
			auto& uniformLayouts = pPipeline->GetUniformLayouts();

			std::vector<VkWriteDescriptorSet> vSetWrites;
			std::vector<VkDescriptorBufferInfo*> pBufferInfos;

			for (auto itr = uniformBuffers.begin(); itr != uniformBuffers.end(); itr++)
			{
				auto& layout = uniformLayouts[itr->first];

				VkDescriptorBufferInfo* pDBI = new VkDescriptorBufferInfo();
				pDBI->buffer = itr->second->Derive<VulkanBuffer>()->vBuffer;
				pDBI->range = layout.mSize;
				pDBI->offset = 0;

				INSERT_INTO_VECTOR(pBufferInfos, pDBI);

				VkWriteDescriptorSet vWDS = {};
				vWDS.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				vWDS.pNext = VK_NULL_HANDLE;
				vWDS.descriptorCount = layout.mLayerCount;
				vWDS.descriptorType = Utilities::GetDescriptorType(layout.mType);
				vWDS.dstBinding = layout.mBinding;
				vWDS.dstSet = vDescriptorSet;
				vWDS.pBufferInfo = pDBI;
				INSERT_INTO_VECTOR(vSetWrites, std::move(vWDS));
			}

			pPipeline->GetRenderTarget()->GetDevice()->Derive<VulkanDevice>()->UpdateDescriptorSet(vSetWrites, {});

			for (auto ptr : pBufferInfos)
				delete ptr;
		}

		void VulkanPipelineResource::Bind(const std::shared_ptr<Backend::CommandBuffer>& pCommandBuffer)
		{
			VulkanPipeline* pvPipeline = pPipeline->Derive<VulkanGraphicsPipeline>()->Derive<VulkanPipeline>();

			vkCmdBindDescriptorSets(
				*dynamic_cast<VulkanCommandBuffer*>(const_cast<Backend::CommandBuffer*>(pCommandBuffer.get())),
				pvPipeline->GetBindPoint(),
				pvPipeline->vPipelineLayout,
				0, 1, &vDescriptorSet, 0, VK_NULL_HANDLE);
		}
		
		void VulkanPipelineResource::CreateDescriptorPool(const std::vector<VkDescriptorPoolSize>& vSizes)
		{
			VkDescriptorPoolCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.maxSets = 1;
			vCI.poolSizeCount = static_cast<UI32>(vSizes.size());
			vCI.pPoolSizes = vSizes.data();

			FLINT_VK_ASSERT(pPipeline->GetRenderTarget()->GetDevice()->Derive<VulkanDevice>()->CreateDescriptorPool(&vCI, &vPool), "Failed to create descriptor pool!")
		}
	}
}