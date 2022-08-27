// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Backend/MeshBindingTable.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Vulkan descriptor set manager class.
		 * This object manages descriptor sets for a single pipeline.
		 *
		 * This works by using the hash of the resource binding table to find the corresponding descriptor set. If a descriptor set for the binding table does not exist, it creates a new one for it
		 * and registers the table.
		 */
		class VulkanDescriptorSetManager
		{
			/**
			 * Descriptor set structure.
			 */
			struct DescriptorSet final
			{
				/**
				 * Default constructor.
				 */
				DescriptorSet() = default;

				/**
				 * Explicit constructor.
				 *
				 * @param copies The descriptor set copies.
				 * @param sets The descriptor sets.
				 */
				explicit DescriptorSet(std::vector<VkCopyDescriptorSet>&& copies, std::vector<VkDescriptorSet>&& sets) : m_CopyDescriptorSets(std::move(copies)), m_DescriptorSets(sets) {}

				std::vector<VkCopyDescriptorSet> m_CopyDescriptorSets;
				std::vector<VkDescriptorSet> m_DescriptorSets;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device to which the manager is bound to.
			 */
			explicit VulkanDescriptorSetManager(const std::shared_ptr<VulkanDevice>& pDevice, uint8_t frameCount);

			/**
			 * Destroy the manager.
			 */
			void destroy();

			/**
			 * Setup the manager.
			 *
			 * @param poolSizes The descriptor pool sizes.
			 * @param layout The descriptor set layout to use.
			 */
			void setup(const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, const std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorSetLayout layout);

			/**
			 * Register a table to the manager.
			 * If a descriptor set exists for the table, it will not do anything.
			 *
			 * @param table The table to register.
			 */
			void registerTable(const MeshBindingTable& table);

			/**
			 * Get the descriptor set from the manager.
			 * Note that this will not create new descriptor sets.
			 *
			 * @param hash The table hash to get the descriptor set from.
			 * @param frameIndex The frame index of the descriptor.
			 * @return The descriptor set.
			 */
			[[nodiscard]] VkDescriptorSet getDescriptorSet(uint64_t hash, uint32_t frameIndex) const;

		protected:
			std::vector<VkDescriptorPoolSize> m_PoolSizes;

			std::unordered_map<uint32_t, VkDescriptorType> m_DescriptorTypeMap;
			std::unordered_map<uint64_t, DescriptorSet> m_DescriptorSets;

			VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
			VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

		private:
			std::shared_ptr<VulkanDevice> m_pDevice = nullptr;
			const uint8_t m_FrameCount = 0;
		};
	}
}