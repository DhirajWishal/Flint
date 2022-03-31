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
		VulkanResourcePackage::VulkanResourcePackage(const std::shared_ptr<VulkanResourcePackager>& pPackager, const std::vector<uint32_t>& bufferBindings, const std::vector<uint32_t>& imageBindings, const VkDescriptorSet& m_vSet)
			: ResourcePackage(pPackager, bufferBindings, imageBindings), m_vDescriptorSet(m_vSet)
		{
		}

		void VulkanResourcePackage::PrepareIfNecessary()
		{
			if (!bIsUpdated)
				return;

			const std::unordered_map<uint32_t, ShaderResourceType> resources = pPackager->GetResources();
			std::vector<VkWriteDescriptorSet> m_vWrites = {};

			VkWriteDescriptorSet m_vWrite = {};
			m_vWrite.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			m_vWrite.pNext = VK_NULL_HANDLE;
			m_vWrite.pImageInfo = VK_NULL_HANDLE;
			m_vWrite.pTexelBufferView = VK_NULL_HANDLE;
			m_vWrite.dstSet = m_vDescriptorSet;

			// Resolve buffers.
			for (const auto buffers : m_BufferBindings)
			{
				m_vWrite.descriptorCount = static_cast<uint32_t>(buffers.second.size());
				m_vWrite.dstArrayElement = 0;
				m_vWrite.dstBinding = buffers.first;
				m_vWrite.descriptorType = Utilities::GetDescriptorType(resources.at(buffers.first));

				uint64_t index = 0;
				VkDescriptorBufferInfo* pBufferInfos = new VkDescriptorBufferInfo[m_vWrite.descriptorCount];
				for (const auto buffer : buffers.second)
				{
					if (!buffer.pBuffer)
						throw backend_error("Requested buffer at binding location: " + std::to_string(buffers.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

					pBufferInfos[index].buffer = buffer.pBuffer->StaticCast<VulkanBuffer>().GetBuffer();
					pBufferInfos[index].offset = buffer.m_Offset;
					pBufferInfos[index].range = VK_WHOLE_SIZE;
					index++;
				}

				m_vWrite.pBufferInfo = pBufferInfos;
				m_vWrites.emplace_back(m_vWrite);
			}

			// Resolve images.
			m_vWrite.pBufferInfo = VK_NULL_HANDLE;
			for (const auto images : m_ImageBindings)
			{
				m_vWrite.descriptorCount = static_cast<uint32_t>(images.second.size());
				m_vWrite.dstArrayElement = 0;
				m_vWrite.dstBinding = images.first;
				m_vWrite.descriptorType = Utilities::GetDescriptorType(resources.at(images.first));

				uint64_t index = 0;
				VkDescriptorImageInfo* pImageInfos = new VkDescriptorImageInfo[m_vWrite.descriptorCount];
				for (const auto image : images.second)
				{
					if (!image.pImage)
						throw backend_error("Requested image at binding location: " + std::to_string(images.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

					if (!image.pImageSampler)
						throw backend_error("Requested image sampler at binding location: " + std::to_string(images.first) + " is not submitted! Make sure that all the bindings are properly submitted.");

					auto& m_vImage = image.pImage->StaticCast<VulkanImage>();
					pImageInfos[index].imageView = image.pImageView->StaticCast<VulkanImageView>().GetImageView();
					pImageInfos[index].imageLayout = m_vImage.GetImageLayout(image.m_Usage);
					pImageInfos[index].sampler = image.pImageSampler->StaticCast<VulkanImageSampler>().GetSampler();
					index++;
				}

				m_vWrite.pImageInfo = pImageInfos;
				m_vWrites.emplace_back(m_vWrite);
			}

			pPackager->GetDevice()->GetDeviceTable().vkUpdateDescriptorSets(pPackager->GetDevice()->GetLogicalDevice(), static_cast<uint32_t>(m_vWrites.size()), m_vWrites.data(), 0, nullptr);

			// Delete the allocated memory.
			for (auto m_vWriteDelete : m_vWrites)
			{
				if (m_vWriteDelete.pBufferInfo)
					delete[] m_vWriteDelete.pBufferInfo;

				else if (m_vWriteDelete.pImageInfo)
					delete[] m_vWriteDelete.pImageInfo;
			}

			bIsUpdated = false;
		}

		void VulkanResourcePackage::SetDescriptorSet(const VkDescriptorSet& m_vSet)
		{
			m_vDescriptorSet = m_vSet;
			bIsUpdated = true;
		}
	}
}