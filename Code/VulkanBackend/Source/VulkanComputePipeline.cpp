// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanComputePipeline.hpp"
#include "VulkanBackend/VulkanShader.hpp"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"
#include "VulkanBackend/VulkanImage.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanImageSampler.hpp"
#include "VulkanBackend/VulkanCommandBuffer.hpp"
#include "VulkanBackend/VulkanCommandBufferAllocator.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanComputePipeline::VulkanComputePipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<Shader>& pComputeShader)
			: ComputePipeline(pDevice, pipelineName, pComputeShader)
		{
			CreatePipelineCache();
			CreatePipelineLayout();
			CreatePipeline();
		}

		void VulkanComputePipeline::ReloadShaders()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			vkDestroyPipeline(vDevice.GetLogicalDevice(), vPipeline, nullptr);
			vkDestroyDescriptorSetLayout(vDevice.GetLogicalDevice(), vDescriptorSetLayout, nullptr);
			vkDestroyPipelineLayout(vDevice.GetLogicalDevice(), vPipelineLayout, nullptr);

			bShouldPrepareResources = true;

			CreatePipelineCache();
			CreatePipelineLayout();
			CreatePipeline();
		}

		void VulkanComputePipeline::Dispatch()
		{
			if (!pThisPipeline)
				pThisPipeline = shared_from_this();

			if (bShouldPrepareResources)
				PrepareResources();

			VulkanCommandBufferAllocator vCommandBuffeAllocator(pDevice, 1);
			auto& vCommandBuffer = vCommandBuffeAllocator.GetCommandBuffers()[0]->StaticCast<VulkanCommandBuffer>();

			vCommandBuffer.BeginBufferRecording();
			vCommandBuffer.BindComputePipeline(pThisPipeline);

			for (UI64 index = 0; index < mInstanceIndex; index++)
			{
				if (mComputeInstances.find(index) == mComputeInstances.end())
					continue;

				const auto instance = mComputeInstances.at(index);

				vCommandBuffer.BindResourceMap(pThisPipeline, instance.pResourceMap);
				vCommandBuffer.BindDynamicStates(pThisPipeline, instance.pDynamicStates);
				vCommandBuffer.IssueComputeCall(instance.mComputeGroups);
			}

			vCommandBuffer.EndBufferRecording();
			pDevice->SubmitComputeCommandBuffers({ vCommandBuffer.shared_from_this() });
			vCommandBuffeAllocator.Terminate();
		}

		void VulkanComputePipeline::Dispatch(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget)
		{
			if (!pThisPipeline)
				pThisPipeline = shared_from_this();

			if (bShouldPrepareResources)
				PrepareResources();
		}

		void VulkanComputePipeline::Dispatch(const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget)
		{
			if (!pThisPipeline)
				pThisPipeline = shared_from_this();

			if (bShouldPrepareResources)
				PrepareResources();
		}

		void VulkanComputePipeline::PrepareResources()
		{
			FLINT_SETUP_PROFILER();
			UI32 descriptorSetCount = static_cast<UI32>(mComputeInstances.size());

			// Check if should prepare resources, and return if not.
			if (!bShouldPrepareResources || mComputeInstances.empty())
				return;

			if (vDescriptorSetPool)
				vkDestroyDescriptorPool(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vDescriptorSetPool, nullptr);

			std::vector<VkDescriptorPoolSize> vPoolSizes;
			std::unordered_map<ShaderResourceKey, ShaderResourceType> resources;

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			// Resolve vertex shader data.
			VulkanShader& vShader = pShader->StaticCast<VulkanShader>();
			Utilities::AddPoolSizesToVector(vPoolSizes, vShader);
			Utilities::AddResourcesToMap(resources, vShader);

			// Return if the pool size is empty.
			if (vPoolSizes.empty())
				return;

			// Create descriptor pool.
			{
				VkDescriptorPoolCreateInfo vPoolCreateInfo = {};
				vPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				vPoolCreateInfo.pNext = VK_NULL_HANDLE;
				vPoolCreateInfo.flags = 0;
				vPoolCreateInfo.maxSets = descriptorSetCount;
				vPoolCreateInfo.poolSizeCount = static_cast<UI32>(vPoolSizes.size());
				vPoolCreateInfo.pPoolSizes = vPoolSizes.data();

				FLINT_VK_ASSERT(vkCreateDescriptorPool(vDevice.GetLogicalDevice(), &vPoolCreateInfo, nullptr, &vDescriptorSetPool));
			}

			// Allocate descriptor sets.
			std::vector<VkDescriptorSet> vDescriptorSets(descriptorSetCount);
			{
				std::vector<VkDescriptorSetLayout> vDescriptorSetLayouts(descriptorSetCount, vDescriptorSetLayout);

				VkDescriptorSetAllocateInfo vAllocateInfo = {};
				vAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				vAllocateInfo.pNext = VK_NULL_HANDLE;
				vAllocateInfo.descriptorPool = vDescriptorSetPool;
				vAllocateInfo.descriptorSetCount = descriptorSetCount;
				vAllocateInfo.pSetLayouts = vDescriptorSetLayouts.data();

				FLINT_VK_ASSERT(vkAllocateDescriptorSets(vDevice.GetLogicalDevice(), &vAllocateInfo, vDescriptorSets.data()));
			}

			// Update descriptor sets.
			std::vector<VkWriteDescriptorSet> vWrites;
			VkWriteDescriptorSet vWrite = {};
			vWrite.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			vWrite.pNext = VK_NULL_HANDLE;
			vWrite.pTexelBufferView = VK_NULL_HANDLE;
			vWrite.descriptorCount = 1;
			vWrite.dstArrayElement = 0;

			UI64 descriptorIndex = 0;
			for (const auto instance : mComputeInstances)
			{
				if (!instance.second.pResourceMap)
					continue;

				vWrite.dstSet = vDescriptorSets[descriptorIndex++];
				vDescriptorSetMap[instance.second.pResourceMap] = vWrite.dstSet;

				vWrite.pImageInfo = VK_NULL_HANDLE;

				// Get buffer resources.
				{
					const auto bufferResources = instance.second.pResourceMap->GetBufferResourceMap();
					for (const auto resource : bufferResources)
					{
						if (!resource.second.pBuffer)
							throw backend_error("No uniform attached to the resource slot {\"" + std::to_string(resource.first.mSetIndex) + ", " + std::to_string(resource.first.mBindingIndex) + "\"}!");

						const ShaderResourceType sResource = resources[resource.first];
						vWrite.descriptorType = Utilities::GetDescriptorType(sResource);
						vWrite.dstBinding = resource.first.mBindingIndex;

						VkDescriptorBufferInfo* pBufferInfo = new VkDescriptorBufferInfo;
						pBufferInfo->buffer = resource.second.pBuffer->StaticCast<VulkanBuffer>().GetBuffer();
						pBufferInfo->range = resource.second.pBuffer->GetSize();
						pBufferInfo->offset = resource.second.mOffset;

						vWrite.pBufferInfo = pBufferInfo;
						INSERT_INTO_VECTOR(vWrites, vWrite);
					}
				}

				vWrite.pBufferInfo = VK_NULL_HANDLE;

				// Get image resources.
				{
					const auto imageResources = instance.second.pResourceMap->GetImageResourceMap();
					for (const auto resource : imageResources)
					{
						if (!resource.second.pImage || !resource.second.pImageSampler)
							throw backend_error("No uniform attached to the resource slot {\"" + std::to_string(resource.first.mSetIndex) + ", " + std::to_string(resource.first.mBindingIndex) + "\"}!");

						const ShaderResourceType sResource = resources[resource.first];
						vWrite.descriptorType = Utilities::GetDescriptorType(sResource);
						vWrite.dstBinding = resource.first.mBindingIndex;

						VulkanImage& vImage = resource.second.pImage->StaticCast<VulkanImage>();
						VkDescriptorImageInfo* pImageInfo = new VkDescriptorImageInfo;
						pImageInfo->imageLayout = vImage.GetImageLayout();
						pImageInfo->imageView = vImage.GetImageView(static_cast<UI32>(resource.second.mViewIndex));
						pImageInfo->sampler = resource.second.pImageSampler->StaticCast<VulkanImageSampler>().GetSampler();

						vWrite.pImageInfo = pImageInfo;
						INSERT_INTO_VECTOR(vWrites, vWrite);
					}
				}
			}

			vkUpdateDescriptorSets(vDevice.GetLogicalDevice(), static_cast<UI32>(vWrites.size()), vWrites.data(), 0, nullptr);

			// Clear memory.
			for (auto write : vWrites)
			{
				if (write.pBufferInfo)
					delete write.pBufferInfo;

				else if (write.pImageInfo)
					delete write.pImageInfo;
			}

			bShouldPrepareResources = false;
		}

		void VulkanComputePipeline::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			// Write cache data.
			UI64 cacheSize = 0;
			FLINT_VK_ASSERT(vkGetPipelineCacheData(vDevice.GetLogicalDevice(), vPipelineCache, &cacheSize, nullptr));

			unsigned char* pDataStore = new unsigned char[cacheSize];
			FLINT_VK_ASSERT(vkGetPipelineCacheData(vDevice.GetLogicalDevice(), vPipelineCache, &cacheSize, pDataStore));

			WriteDataToCacheFile(cacheSize, pDataStore);
			delete[] pDataStore;

			// Delete pipeline objects.
			vkDestroyPipeline(vDevice.GetLogicalDevice(), vPipeline, nullptr);
			vkDestroyDescriptorSetLayout(vDevice.GetLogicalDevice(), vDescriptorSetLayout, nullptr);
			vkDestroyPipelineLayout(vDevice.GetLogicalDevice(), vPipelineLayout, nullptr);
			vkDestroyPipelineCache(vDevice.GetLogicalDevice(), vPipelineCache, nullptr);

			if (vDescriptorSetPool)
				vkDestroyDescriptorPool(vDevice.GetLogicalDevice(), vDescriptorSetPool, nullptr);

			bIsTerminated = true;
		}

		const VkDescriptorSet VulkanComputePipeline::GetDescriptorSet(const std::shared_ptr<ResourceMap>& pResourceMap) const
		{
			if (vDescriptorSetMap.find(pResourceMap) == vDescriptorSetMap.end())
				return VK_NULL_HANDLE;

			return vDescriptorSetMap.at(pResourceMap);
		}

		const VkDescriptorSet* VulkanComputePipeline::GetDescriptorSetAddress(const std::shared_ptr<ResourceMap>& pResourceMap) const
		{
			if (vDescriptorSetMap.find(pResourceMap) == vDescriptorSetMap.end())
				return nullptr;

			return &vDescriptorSetMap.at(pResourceMap);
		}

		void VulkanComputePipeline::CreatePipelineCache()
		{
			FLINT_SETUP_PROFILER();

			auto [size, data] = ReadDataFromCacheFile();

			VkPipelineCacheCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.initialDataSize = size;
			vCreateInfo.pInitialData = data;

			FLINT_VK_ASSERT(vkCreatePipelineCache(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCreateInfo, nullptr, &vPipelineCache));

			delete[] data;
		}

		void VulkanComputePipeline::CreatePipelineLayout()
		{
			FLINT_SETUP_PROFILER();

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			std::vector<VkDescriptorSetLayoutBinding> vBindings;
			std::vector<VkPushConstantRange> vConstantRanges;

			// Resolve compute shader data.
			Utilities::AddResourceBindingsToVector(vBindings, pShader->StaticCast<VulkanShader>());
			Utilities::AddPushConstantRangesToVector(vConstantRanges, pShader->StaticCast<VulkanShader>());

			// Create descriptor set layout.
			{
				VkDescriptorSetLayoutCreateInfo vLayoutCreateInfo = {};
				vLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				vLayoutCreateInfo.pNext = VK_NULL_HANDLE;
				vLayoutCreateInfo.flags = 0;
				vLayoutCreateInfo.bindingCount = static_cast<UI32>(vBindings.size());
				vLayoutCreateInfo.pBindings = vBindings.data();

				FLINT_VK_ASSERT(vkCreateDescriptorSetLayout(vDevice.GetLogicalDevice(), &vLayoutCreateInfo, nullptr, &vDescriptorSetLayout));
			}

			// Create pipeline layout.
			VkPipelineLayoutCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.pushConstantRangeCount = static_cast<UI32>(vConstantRanges.size());
			vCreateInfo.pPushConstantRanges = vConstantRanges.data();
			vCreateInfo.setLayoutCount = 1;
			vCreateInfo.pSetLayouts = &vDescriptorSetLayout;

			FLINT_VK_ASSERT(vkCreatePipelineLayout(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vPipelineLayout));
		}

		void VulkanComputePipeline::CreatePipeline()
		{
			VkComputePipelineCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.layout = vPipelineLayout;
			vCreateInfo.basePipelineIndex = 0;
			vCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			vCreateInfo.stage = pShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo();

			FLINT_VK_ASSERT(vkCreateComputePipelines(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vPipelineCache, 1, &vCreateInfo, nullptr, &vPipeline));
		}
	}
}