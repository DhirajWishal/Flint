// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanDescriptorSetManager.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "Core/Utility/Hasher.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanDescriptorSetManager::VulkanDescriptorSetManager(VulkanEngine& engine)
			: m_Engine(engine)
		{
		}

		VulkanDescriptorSetManager::~VulkanDescriptorSetManager()
		{
			m_Engine.getDeviceTable().vkDestroyDescriptorSetLayout(m_Engine.getLogicalDevice(), m_DescriptorSetLayout, nullptr);

			if (m_DescriptorPool != VK_NULL_HANDLE)
				m_Engine.getDeviceTable().vkDestroyDescriptorPool(m_Engine.getLogicalDevice(), m_DescriptorPool, nullptr);
		}

		void VulkanDescriptorSetManager::setup(std::vector<VkDescriptorSetLayoutBinding>&& layoutBindings)
		{
			// Create the descriptor type map.
			for (const auto& binding : layoutBindings)
				m_DescriptorTypeMap[binding.binding] = binding.descriptorType;

			// Create the layout.
			VkDescriptorSetLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
			createInfo.pBindings = layoutBindings.data();

			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkCreateDescriptorSetLayout(m_Engine.getLogicalDevice(), &createInfo, nullptr, &m_DescriptorSetLayout), "Failed to create the descriptor set layout!");
		}

		void VulkanDescriptorSetManager::registerTable(const ResourceBindingTable& table)
		{
			const auto tableHash = GenerateHash(table);

			// Return if the table is registered.
			if (m_DescriptorSets.contains(tableHash))
				return;

			// Else we can create a new one and update the previous descriptors.
			// First, create the new descriptor pool.
			VkDescriptorPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.maxSets = static_cast<uint32_t>(m_DescriptorSets.size()) + 1;
			createInfo.poolSizeCount = static_cast<uint32_t>(m_PoolSizes.size());
			createInfo.pPoolSizes = m_PoolSizes.data();

			VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkCreateDescriptorPool(m_Engine.getLogicalDevice(), &createInfo, nullptr, &descriptorPool), "Failed to create the descriptor pool!");

			// Allocate the descriptor sets.
			VkDescriptorSetAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.descriptorSetCount = createInfo.maxSets;
			allocateInfo.pSetLayouts = &m_DescriptorSetLayout;
			allocateInfo.descriptorPool = descriptorPool;

			// Create a descriptor set and copy the old stuff to it.
			for (auto& [hash, table] : m_DescriptorSets)
			{
				VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
				FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkAllocateDescriptorSets(m_Engine.getLogicalDevice(), &allocateInfo, &descriptorSet), "Failed to allocate descriptor set!");

				// Set the destination set.
				for (auto& copyInfo : table.m_CopyDescriptorSets)
					copyInfo.dstSet = descriptorSet;

				m_Engine.getDeviceTable().vkUpdateDescriptorSets(m_Engine.getLogicalDevice(), 0, nullptr, static_cast<uint32_t>(table.m_CopyDescriptorSets.size()), table.m_CopyDescriptorSets.data());
				table.m_DescriptorSet = descriptorSet;

				// Set the source set.
				for (auto& copyInfo : table.m_CopyDescriptorSets)
					copyInfo.srcSet = descriptorSet;
			}

			// Delete the old descriptor set.
			if (m_DescriptorPool != VK_NULL_HANDLE)
				m_Engine.getDeviceTable().vkDestroyDescriptorPool(m_Engine.getLogicalDevice(), m_DescriptorPool, nullptr);
			m_DescriptorPool = descriptorPool;

			// Now we can create the new one.
			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(m_Engine.getDeviceTable().vkAllocateDescriptorSets(m_Engine.getLogicalDevice(), &allocateInfo, &descriptorSet), "Failed to allocate descriptor set!");

			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
			std::vector<VkCopyDescriptorSet> copyDescriptorSets;

			// Resolve the images.
			for (const auto& [binding, images] : table.m_Images)
			{
				const auto imageCount = static_cast<uint32_t>(images.size());
				for (uint32_t i = 0; i < imageCount; i++)
				{
					// Setup write info.
					auto& writeDescriptorSet = writeDescriptorSets.emplace_back();
					writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					writeDescriptorSet.pNext = nullptr;
					writeDescriptorSet.dstSet = 0;
					writeDescriptorSet.dstBinding = binding;
					writeDescriptorSet.descriptorCount = imageCount;
					writeDescriptorSet.descriptorType = m_DescriptorTypeMap[binding];
					writeDescriptorSet.pBufferInfo = nullptr;
					writeDescriptorSet.pTexelBufferView = nullptr;
					writeDescriptorSet.dstArrayElement = i;

					writeDescriptorSet.pImageInfo = new VkDescriptorImageInfo;
					writeDescriptorSet.pImageInfo->imageLayout;
					writeDescriptorSet.pImageInfo->imageView;
					writeDescriptorSet.pImageInfo->sampler;

					// Setup copy info.
					auto& copySet = copyDescriptorSets.emplace_back();
					copySet.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
					copySet.pNext = nullptr;
					copySet.dstSet = 0;
					copySet.srcSet = 0;
					copySet.descriptorCount = imageCount;
					copySet.dstBinding = binding;
					copySet.dstArrayElement = i;
					copySet.srcBinding = binding;
					copySet.srcArrayElement = i;
				}
			}

			// Resolve buffers.
			for (const auto& [binding, buffers] : table.m_Buffers)
			{
				const auto bufferCount = static_cast<uint32_t>(buffers.size());
				for (uint32_t i = 0; i < bufferCount; i++)
				{
					// Setup write info.
					auto& writeDescriptorSet = writeDescriptorSets.emplace_back();
					writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					writeDescriptorSet.pNext = nullptr;
					writeDescriptorSet.dstSet = descriptorSet;
					writeDescriptorSet.dstBinding = binding;
					writeDescriptorSet.descriptorCount = bufferCount;
					writeDescriptorSet.descriptorType = m_DescriptorTypeMap[binding];
					writeDescriptorSet.dstArrayElement = i;
					writeDescriptorSet.pBufferInfo = m_Engine.getBuffer(buffers[i]).getDescriptorBufferInfo();
					writeDescriptorSet.pImageInfo = nullptr;
					writeDescriptorSet.pTexelBufferView = nullptr;

					// Setup copy info.
					auto& copySet = copyDescriptorSets.emplace_back();
					copySet.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
					copySet.pNext = nullptr;
					copySet.dstSet = 0;
					copySet.srcSet = descriptorSet;
					copySet.descriptorCount = bufferCount;
					copySet.dstBinding = binding;
					copySet.dstArrayElement = i;
					copySet.srcBinding = binding;
					copySet.srcArrayElement = i;
				}
			}

			// Update the descriptor set with the data.
			m_Engine.getDeviceTable().vkUpdateDescriptorSets(m_Engine.getLogicalDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

			// Delete the allocated memory.
			for (auto& write : writeDescriptorSets)
			{
				if (write.pImageInfo)
					delete write.pImageInfo;
			}

			// Add the descriptor set to the list.
			m_DescriptorSets.emplace(tableHash, DescriptorSet(std::move(copyDescriptorSets), descriptorSet));
		}

		VkDescriptorSet VulkanDescriptorSetManager::getDescriptorSet(const ResourceBindingTable& table) const
		{
			return m_DescriptorSets.at(GenerateHash(table)).m_DescriptorSet;
		}

		void VulkanDescriptorSetManager::copyDescriptorSet(VkDescriptorSet source, VkDescriptorSet destination, const ResourceBindingTable& table) const
		{
			VkCopyDescriptorSet copySet = {};
			copySet.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
			copySet.pNext = nullptr;
			copySet.descriptorCount;
			copySet.dstSet = 0;
			copySet.dstBinding;
			copySet.dstArrayElement;
			copySet.srcSet = 0;
			copySet.srcBinding;
			copySet.srcArrayElement;
		}
	}
}