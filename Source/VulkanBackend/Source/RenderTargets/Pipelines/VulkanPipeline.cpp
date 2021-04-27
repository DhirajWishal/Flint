// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/Pipelines/VulkanPipeline.h"
#include "VulkanBackend/RenderTargets/Pipelines/VulkanGraphicsPipeline.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/VulkanBuffer.h"
#include "VulkanBackend/VulkanUtilities.h"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanPipelineResource::VulkanPipelineResource(std::shared_ptr<FPipeline> pPipeline)
			: FPipelineResource(pPipeline)
		{
			CreateDescriptorPool(pPipeline->GetAs<VulkanPipeline>()->vPoolSizes);

			VkDescriptorSetAllocateInfo vDSAI = {};
			vDSAI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			vDSAI.pNext = VK_NULL_HANDLE;
			vDSAI.descriptorSetCount = 1;
			vDSAI.pSetLayouts = &pPipeline->GetAs<VulkanPipeline>()->vSetLayout;
			vDSAI.descriptorPool = vPool;

			FLINT_VK_ASSERT(pPipeline->GetAs<VulkanPipeline>()->GetRenderTarget()->GetDevice()->AllocateDescriptorSet(&vDSAI, &vDescriptorSet), "Failed to allocate descriptor set!");
		}

		VulkanPipelineResource::~VulkanPipelineResource()
		{
			DeviceType* pDevice = pPipeline->GetAs<VulkanPipeline>()->GetRenderTarget()->GetDevice();
			pDevice->DestroyDescriptorPool(vPool);

			for (auto& samplerPair : mSamplerMap)
				pDevice->DestroySampler(samplerPair.second);

			mSamplerMap.clear();
		}

		void VulkanPipelineResource::RegisterUniformBuffers(const UniformBufferContainer& uniformBuffers)
		{
			auto& uniformLayouts = pPipeline->GetAs<VulkanPipeline>()->GetUniformLayouts();

			std::vector<VkWriteDescriptorSet> vSetWrites;
			std::vector<VkDescriptorBufferInfo*> pBufferInfos;

			for (auto itr = uniformBuffers.begin(); itr != uniformBuffers.end(); itr++)
			{
				auto& layout = uniformLayouts[itr->first];

				VkDescriptorBufferInfo* pDBI = new VkDescriptorBufferInfo();
				pDBI->buffer = itr->second->GetAs<VulkanBuffer>()->vBuffer;
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

			pPipeline->GetAs<VulkanPipeline>()->GetRenderTarget()->GetDevice()->UpdateDescriptorSet(vSetWrites, {});

			for (auto ptr : pBufferInfos)
				delete ptr;
		}

		void VulkanPipelineResource::RegisterUniformImages(const UniformImageContainer& unformImages)
		{
			auto& uniformLayouts = pPipeline->GetUniformLayouts();

			std::vector<VkWriteDescriptorSet> vSetWrites;
			std::vector<VkDescriptorImageInfo*> pImageInfos;

			for (auto itr = unformImages.begin(); itr != unformImages.end(); itr++)
			{
				auto& layout = uniformLayouts[itr->first];
				VulkanImage* pImage = itr->second.first->GetAs<VulkanImage>();

				VkDescriptorImageInfo* pDII = new VkDescriptorImageInfo();
				pDII->imageLayout = pImage->GetLayout();
				pDII->imageView = pImage->GetImageView();

				if (mSamplerMap.find(itr->second.second) != mSamplerMap.end())
					pDII->sampler = mSamplerMap[itr->second.second];
				else
					pDII->sampler = itr->second.first->GetAs<VulkanImage>()->CreateSampler(itr->second.second), mSamplerMap[itr->second.second] = pDII->sampler;

				INSERT_INTO_VECTOR(pImageInfos, pDII);

				VkWriteDescriptorSet vWDS = {};
				vWDS.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				vWDS.pNext = VK_NULL_HANDLE;
				vWDS.descriptorCount = layout.mLayerCount;
				vWDS.descriptorType = Utilities::GetDescriptorType(layout.mType);
				vWDS.dstBinding = layout.mBinding;
				vWDS.dstSet = vDescriptorSet;
				vWDS.pImageInfo = pDII;
				INSERT_INTO_VECTOR(vSetWrites, std::move(vWDS));
			}

			pPipeline->GetAs<VulkanPipeline>()->GetRenderTarget()->GetDevice()->UpdateDescriptorSet(vSetWrites, {});

			for (auto ptr : pImageInfos)
				delete ptr;
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

			FLINT_VK_ASSERT(pPipeline->GetAs<VulkanPipeline>()->GetRenderTarget()->GetDevice()->CreateDescriptorPool(&vCI, &vPool), "Failed to create descriptor pool!");
		}
	}
}