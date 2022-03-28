// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanResourcePackage.hpp"
#include "VulkanBackend/VulkanResourcePackager.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanImage.hpp"
#include "VulkanBackend/VulkanImageView.hpp"
#include "VulkanBackend/VulkanImageSampler.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanResourcePackage::VulkanResourcePackage(const std::shared_ptr<VulkanResourcePackager>& pPackager, const std::vector<uint32_t>& bufferBindings, const std::vector<uint32_t>& imageBindings, const VkDescriptorSet& vSet)
			: ResourcePackage(pPackager, bufferBindings, imageBindings), vDescriptorSet(vSet)
		{
		}

		void VulkanResourcePackage::PrepareIfNecessary()
		{
			if (!bIsUpdated)
				return;

			const std::unordered_map<uint32_t, ShaderResourceType> resources = pPackager->GetResources();
			std::vector<VkWriteDescriptorSet> vWrites = {};

			VkWriteDescriptorSet vWrite = {};
			vWrite.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			vWrite.pNext = VK_NULL_HANDLE;
			vWrite.pImageInfo = VK_NULL_HANDLE;
			vWrite.pTexelBufferView = VK_NULL_HANDLE;
			vWrite.dstSet = vDescriptorSet;

			// Resolve buffers.
			for (const auto buffers : m_BufferBindings)
			{
				vWrite.descriptorCount = static_cast<uint32_t>(buffers.second.size());
				vWrite.dstArrayElement = 0;
				vWrite.dstBinding = buffers.first;
				vWrite.descriptorType = Utilities::GetDescriptorType(resources.at(buffers.first));

				uint64_t index = 0;
				VkDescriptorBufferInfo* pBufferInfos = new VkDescriptorBufferInfo[vWrite.descriptorCount];
				for (const auto buffer : buffers.second)
				{
					if (!buffer.pBuffer)
						throw backend_error("Requested buffer at binding location: " + std::to_string(buffers.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

					pBufferInfos[index].buffer = buffer.pBuffer->StaticCast<VulkanBuffer>().GetBuffer();
					pBufferInfos[index].offset = buffer.m_Offset;
					pBufferInfos[index].range = VK_WHOLE_SIZE;
					index++;
				}

				vWrite.pBufferInfo = pBufferInfos;
				vWrites.emplace_back(vWrite);
			}

			// Resolve images.
			vWrite.pBufferInfo = VK_NULL_HANDLE;
			for (const auto images : m_ImageBindings)
			{
				vWrite.descriptorCount = static_cast<uint32_t>(images.second.size());
				vWrite.dstArrayElement = 0;
				vWrite.dstBinding = images.first;
				vWrite.descriptorType = Utilities::GetDescriptorType(resources.at(images.first));

				uint64_t index = 0;
				VkDescriptorImageInfo* pImageInfos = new VkDescriptorImageInfo[vWrite.descriptorCount];
				for (const auto image : images.second)
				{
					if (!image.pImage)
						throw backend_error("Requested image at binding location: " + std::to_string(images.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

					if (!image.pImageSampler)
						throw backend_error("Requested image sampler at binding location: " + std::to_string(images.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

					auto& vImage = image.pImage->StaticCast<VulkanImage>();
					pImageInfos[index].imageView = image.pImageView->StaticCast<VulkanImageView>().GetImageView();
					pImageInfos[index].imageLayout = vImage.GetImageLayout(image.m_Usage);
					pImageInfos[index].sampler = image.pImageSampler->StaticCast<VulkanImageSampler>().GetSampler();
					index++;
				}

				vWrite.pImageInfo = pImageInfos;
				vWrites.emplace_back(vWrite);
			}

			pPackager->GetDevice()->GetDeviceTable().vkUpdateDescriptorSets(pPackager->GetDevice()->GetLogicalDevice(), static_cast<uint32_t>(vWrites.size()), vWrites.data(), 0, nullptr);

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