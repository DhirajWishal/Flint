// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/VulkanBackend/VulkanDescriptorSetManager.hpp"
#include "Flint/VulkanBackend/VulkanMacros.hpp"
#include "Flint/VulkanBackend/VulkanBuffer.hpp"
#include "Flint/VulkanBackend/VulkanTextureSampler.hpp"
#include "Flint/VulkanBackend/VulkanTextureView.hpp"

#include <Optick.h>

namespace Flint
{
	namespace Backend
	{
		VulkanDescriptorSetManager::VulkanDescriptorSetManager(const std::shared_ptr<VulkanDevice>& pDevice, uint8_t frameCount)
			: m_pDevice(pDevice), m_FrameCount(frameCount)
		{
		}

		void VulkanDescriptorSetManager::destroy()
		{
			if (m_DescriptorPool != VK_NULL_HANDLE)
				m_pDevice->getDeviceTable().vkDestroyDescriptorPool(m_pDevice->getLogicalDevice(), m_DescriptorPool, nullptr);
		}

		void VulkanDescriptorSetManager::setup(const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, const std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorSetLayout layout)
		{
			OPTICK_EVENT();

			// Create the descriptor type map.
			for (const auto& binding : layoutBindings)
				m_DescriptorTypeMap[binding.binding] = binding.descriptorType;

			m_PoolSizes = std::move(poolSizes);
			m_DescriptorSetLayout = layout;
		}

		void VulkanDescriptorSetManager::registerTable(const MeshBindingTable& table)
		{
			OPTICK_EVENT();

			const auto tableHash = table.generateHash();

			// Return if the table is registered.
			if (m_DescriptorSets.contains(tableHash))
				return;

			// Else we can create a new one and update the previous descriptors.
			// First, create the new descriptor pool.
			VkDescriptorPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.maxSets = (static_cast<uint32_t>(m_DescriptorSets.size()) + 1) * m_FrameCount;
			createInfo.poolSizeCount = static_cast<uint32_t>(m_PoolSizes.size());
			createInfo.pPoolSizes = m_PoolSizes.data();

			VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateDescriptorPool(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &descriptorPool), "Failed to create the descriptor pool!");

			// Allocate the descriptor sets.
			VkDescriptorSetAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.descriptorSetCount = 1;
			allocateInfo.pSetLayouts = &m_DescriptorSetLayout;
			allocateInfo.descriptorPool = descriptorPool;

			// Create a descriptor set and copy the old stuff to it.
			for (auto& [hash, table] : m_DescriptorSets)
			{
				for (uint8_t i = 0; i < m_FrameCount; i++)
				{
					VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
					FLINT_VK_ASSERT(m_pDevice->getDeviceTable().vkAllocateDescriptorSets(m_pDevice->getLogicalDevice(), &allocateInfo, &descriptorSet), "Failed to allocate descriptor set!");

					// Set the destination set.
					for (auto& copyInfo : table.m_CopyDescriptorSets)
						copyInfo.dstSet = descriptorSet;

					m_pDevice->getDeviceTable().vkUpdateDescriptorSets(m_pDevice->getLogicalDevice(), 0, nullptr, static_cast<uint32_t>(table.m_CopyDescriptorSets.size()), table.m_CopyDescriptorSets.data());
					table.m_DescriptorSets[i] = descriptorSet;

					// Set the source set.
					for (auto& copyInfo : table.m_CopyDescriptorSets)
						copyInfo.srcSet = descriptorSet;
				}
			}

			// Delete the old descriptor set.
			if (m_DescriptorPool != VK_NULL_HANDLE)
				m_pDevice->getDeviceTable().vkDestroyDescriptorPool(m_pDevice->getLogicalDevice(), m_DescriptorPool, nullptr);
			m_DescriptorPool = descriptorPool;

			// Now we can create the new ones.
			std::vector<VkDescriptorSetLayout> layouts(m_FrameCount, m_DescriptorSetLayout);
			std::vector<VkDescriptorSet> descriptorSets(m_FrameCount);

			allocateInfo.descriptorSetCount = m_FrameCount;
			allocateInfo.pSetLayouts = layouts.data();
			FLINT_VK_ASSERT(m_pDevice->getDeviceTable().vkAllocateDescriptorSets(m_pDevice->getLogicalDevice(), &allocateInfo, descriptorSets.data()), "Failed to allocate descriptor set!");

			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
			std::vector<VkCopyDescriptorSet> copyDescriptorSets;

			// Resolve the images.
			for (const auto& [binding, image] : table.getImages())
			{
				// Setup write info.
				auto& writeDescriptorSet = writeDescriptorSets.emplace_back();
				writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSet.pNext = nullptr;
				writeDescriptorSet.dstSet = descriptorSets.front();
				writeDescriptorSet.dstBinding = binding;
				writeDescriptorSet.descriptorCount = 1;
				writeDescriptorSet.descriptorType = m_DescriptorTypeMap[binding];
				writeDescriptorSet.pBufferInfo = nullptr;
				writeDescriptorSet.pTexelBufferView = nullptr;
				writeDescriptorSet.dstArrayElement = 0;

				auto pImageInfo = new VkDescriptorImageInfo;
				pImageInfo->imageLayout = image.m_ImageUsage == ImageUsage::Graphics ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				pImageInfo->imageView = image.m_pTextureView->as<VulkanTextureView>()->getViewHandle();
				pImageInfo->sampler = image.m_pTextureSampler->as<VulkanTextureSampler>()->getSamplerHandle();
				writeDescriptorSet.pImageInfo = pImageInfo;

				// Setup copy info.
				auto& copySet = copyDescriptorSets.emplace_back();
				copySet.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
				copySet.pNext = nullptr;
				copySet.dstSet = VK_NULL_HANDLE;
				copySet.srcSet = descriptorSets.front();
				copySet.descriptorCount = 1;
				copySet.dstBinding = binding;
				copySet.dstArrayElement = 0;
				copySet.srcBinding = binding;
				copySet.srcArrayElement = 0;
			}

			// Resolve buffers.
			for (const auto& [binding, buffers] : table.getBuffers())
			{
				// Setup write info.
				auto& writeDescriptorSet = writeDescriptorSets.emplace_back();
				writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSet.pNext = nullptr;
				writeDescriptorSet.dstSet = descriptorSets.front();
				writeDescriptorSet.dstBinding = binding;
				writeDescriptorSet.descriptorCount = 1;
				writeDescriptorSet.descriptorType = m_DescriptorTypeMap[binding];
				writeDescriptorSet.dstArrayElement = 0;
				writeDescriptorSet.pBufferInfo = buffers->as<VulkanBuffer>()->getDescriptorBufferInfo();
				writeDescriptorSet.pImageInfo = nullptr;
				writeDescriptorSet.pTexelBufferView = nullptr;

				// Setup copy info.
				auto& copySet = copyDescriptorSets.emplace_back();
				copySet.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
				copySet.pNext = nullptr;
				copySet.dstSet = VK_NULL_HANDLE;
				copySet.srcSet = descriptorSets.front();
				copySet.descriptorCount = 1;
				copySet.dstBinding = binding;
				copySet.dstArrayElement = 0;
				copySet.srcBinding = binding;
				copySet.srcArrayElement = 0;
			}

			// Update the descriptor sets with the data.
			m_pDevice->getDeviceTable().vkUpdateDescriptorSets(m_pDevice->getLogicalDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

			// Copy the descriptors to the rest.
			for (uint8_t i = 1; i < m_FrameCount; i++)
			{
				for (auto& copy : copyDescriptorSets)
					copy.dstSet = descriptorSets[i];

				m_pDevice->getDeviceTable().vkUpdateDescriptorSets(m_pDevice->getLogicalDevice(), 0, nullptr, static_cast<uint32_t>(copyDescriptorSets.size()), copyDescriptorSets.data());
			}

			// Delete the allocated memory.
			for (auto& write : writeDescriptorSets)
			{
				if (write.pImageInfo)
					delete write.pImageInfo;
			}

			// Add the descriptor set to the list.
			m_DescriptorSets.emplace(tableHash, DescriptorSet(std::move(copyDescriptorSets), std::move(descriptorSets)));
		}

		VkDescriptorSet VulkanDescriptorSetManager::getDescriptorSet(uint64_t hash, uint32_t frameIndex) const
		{
			OPTICK_EVENT();

			return m_DescriptorSets.at(hash).m_DescriptorSets[frameIndex];
		}
	}
}