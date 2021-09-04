// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanResourcePackage.hpp"
#include "VulkanBackend/VulkanResourcePackager.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanImage.hpp"
#include "VulkanBackend/VulkanImageSampler.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanResourcePackage::VulkanResourcePackage(const std::shared_ptr<ResourcePackager>& pPackager, const std::vector<UI32>& bufferBindings, const std::vector<UI32>& imageBindings, const VkDescriptorSet& vSet)
			: ResourcePackage(pPackager, bufferBindings, imageBindings), vDescriptorSet(vSet)
		{
		}

		void VulkanResourcePackage::PrepareIfNecessary()
		{
			if (!bIsUpdated)
				return;

			const std::unordered_map<UI32, ShaderResourceType> resources = pPackager->GetResources();
			std::vector<VkWriteDescriptorSet> vWrites = {};

			VkWriteDescriptorSet vWrite = {};
			vWrite.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			vWrite.pNext = VK_NULL_HANDLE;
			vWrite.pImageInfo = VK_NULL_HANDLE;
			vWrite.pTexelBufferView = VK_NULL_HANDLE;
			vWrite.dstSet = vDescriptorSet;
			vWrite.descriptorCount = 1;	// TODO

			// Resolve buffers.
			for (const auto buffer : mBufferBindings)
			{
				if (!buffer.second.pBuffer)
					throw backend_error("Requested buffer at binding location: " + std::to_string(buffer.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

				vWrite.dstBinding = buffer.first;
				vWrite.dstArrayElement = buffer.second.mArrayIndex;
				vWrite.descriptorType = Utilities::GetDescriptorType(resources.at(buffer.first));

				VkDescriptorBufferInfo* pBufferInfo = new VkDescriptorBufferInfo();
				pBufferInfo->buffer = buffer.second.pBuffer->StaticCast<VulkanBuffer>().GetBuffer();
				pBufferInfo->offset = buffer.second.mOffset;
				pBufferInfo->range = VK_WHOLE_SIZE;

				vWrite.pBufferInfo = pBufferInfo;
				vWrites.push_back(vWrite);
			}

			// Resolve images.
			vWrite.pBufferInfo = VK_NULL_HANDLE;
			for (const auto image : mImageBindings)
			{
				if (!image.second.pImage || !image.second.pImageSampler)
					throw backend_error("Requested image sampler at binding location: " + std::to_string(image.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

				vWrite.dstBinding = image.first;
				vWrite.dstArrayElement = image.second.mArrayIndex;
				vWrite.descriptorType = Utilities::GetDescriptorType(resources.at(image.first));

				auto& vImage = image.second.pImage->StaticCast<VulkanImage>();
				VkDescriptorImageInfo* pImageInfo = new VkDescriptorImageInfo();
				pImageInfo->imageView = vImage.GetImageView(image.second.mViewIndex);
				pImageInfo->imageLayout = vImage.GetAttachmentLayout();
				pImageInfo->sampler = image.second.pImageSampler->StaticCast<VulkanImageSampler>().GetSampler();

				vWrite.pImageInfo = pImageInfo;
				vWrites.push_back(vWrite);
			}

			vkUpdateDescriptorSets(pPackager->GetPipeline()->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), static_cast<UI32>(vWrites.size()), vWrites.data(), 0, nullptr);

			// Delete the allocated memory.
			for (auto vWriteDelete : vWrites)
			{
				if (vWriteDelete.pBufferInfo)
					delete vWriteDelete.pBufferInfo;
				else if (vWriteDelete.pImageInfo)
					delete vWriteDelete.pImageInfo;
			}

			bIsUpdated = true;
		}

		void VulkanResourcePackage::SetDescriptorSet(const VkDescriptorSet& vSet)
		{
			vDescriptorSet = vSet;
			bIsUpdated = true;
		}
	}
}