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

			VulkanDevice& vDevice = pPackager->GetPipeline()->GetDevice()->StaticCast<VulkanDevice>();
			const std::unordered_map<UI32, ShaderResourceType> resources = pPackager->GetResources();
			std::vector<VkWriteDescriptorSet> vWrites = {};

			VkWriteDescriptorSet vWrite = {};
			vWrite.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			vWrite.pNext = VK_NULL_HANDLE;
			vWrite.pImageInfo = VK_NULL_HANDLE;
			vWrite.pTexelBufferView = VK_NULL_HANDLE;
			vWrite.dstSet = vDescriptorSet;

			// Resolve buffers.
			for (const auto buffers : mBufferBindings)
			{
				vWrite.descriptorCount = static_cast<UI32>(buffers.second.size());
				vWrite.dstArrayElement = -1;
				vWrite.dstBinding = buffers.first;
				vWrite.descriptorType = Utilities::GetDescriptorType(resources.at(buffers.first));

				VkDescriptorBufferInfo* pBufferInfos = new VkDescriptorBufferInfo[vWrite.descriptorCount];
				for (const auto buffer : buffers.second)
				{
					if (!buffer.pBuffer)
						throw backend_error("Requested buffer at binding location: " + std::to_string(buffers.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

					vWrite.dstArrayElement++;
					pBufferInfos[vWrite.dstArrayElement].buffer = buffer.pBuffer->StaticCast<VulkanBuffer>().GetBuffer();
					pBufferInfos[vWrite.dstArrayElement].offset = buffer.mOffset;
					pBufferInfos[vWrite.dstArrayElement].range = VK_WHOLE_SIZE;
				}

				vWrite.pBufferInfo = pBufferInfos;
				vWrites.push_back(vWrite);
			}

			// Resolve images.
			vWrite.pBufferInfo = VK_NULL_HANDLE;
			for (const auto images : mImageBindings)
			{
				vWrite.descriptorCount = static_cast<UI32>(images.second.size());
				vWrite.dstArrayElement = -1;
				vWrite.dstBinding = images.first;
				vWrite.descriptorType = Utilities::GetDescriptorType(resources.at(images.first));

				VkDescriptorImageInfo* pImageInfos = new VkDescriptorImageInfo[vWrite.descriptorCount];
				for (const auto image : images.second)
				{
					if (!image.pImage)
						throw backend_error("Requested image at binding location: " + std::to_string(images.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

					if (!image.pImageSampler)
						throw backend_error("Requested image sampler at binding location: " + std::to_string(images.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

					auto& vImage = image.pImage->StaticCast<VulkanImage>();

					vWrite.dstArrayElement++;
					pImageInfos[vWrite.dstArrayElement].imageView = vImage.GetImageView(static_cast<UI32>(image.mViewIndex));
					pImageInfos[vWrite.dstArrayElement].imageLayout = vImage.GetImageLayout(image.mUsage);
					pImageInfos[vWrite.dstArrayElement].sampler = image.pImageSampler->StaticCast<VulkanImageSampler>().GetSampler();
				}

				vWrite.pImageInfo = pImageInfos;
				vWrites.push_back(vWrite);
			}

			vDevice.GetDeviceTable().vkUpdateDescriptorSets(vDevice.GetLogicalDevice(), static_cast<UI32>(vWrites.size()), vWrites.data(), 0, nullptr);

			// Delete the allocated memory.
			for (auto vWriteDelete : vWrites)
			{
				if (vWriteDelete.pBufferInfo)
					delete[] vWriteDelete.pBufferInfo;

				else if (vWriteDelete.pImageInfo)
					delete[] vWriteDelete.pImageInfo;
			}

			bIsUpdated = false;
		}

		void VulkanResourcePackage::SetDescriptorSet(const VkDescriptorSet& vSet)
		{
			vDescriptorSet = vSet;
			bIsUpdated = true;
		}
	}
}