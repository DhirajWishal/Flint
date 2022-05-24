// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanEngine.hpp"

#include <unordered_map>

namespace Flint
{
	namespace VulkanBackend
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
				constexpr DescriptorSet() = default;

				/**
				 * Explicit constructor.
				 *
				 * @param copies The descriptor set copies.
				 * @param set The descriptor set.
				 */
				explicit DescriptorSet(std::vector<VkCopyDescriptorSet>&& copies, VkDescriptorSet set) : m_CopyDescriptorSets(std::move(copies)), m_DescriptorSet(set) {}

				std::vector<VkCopyDescriptorSet> m_CopyDescriptorSets;
				VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param engine The engine to which the manager is bound to.
			 */
			explicit VulkanDescriptorSetManager(VulkanEngine& engine);

			/**
			 * Destructor.
			 */
			~VulkanDescriptorSetManager();

			/**
			 * Setup the manager.
			 *
			 * @param layoutBindings The layout bindings.
			 */
			void setup(std::vector<VkDescriptorSetLayoutBinding>&& layoutBindings);

			/**
			 * Register a table to the manager.
			 * If a descriptor set exists for the table, it will not do anything.
			 *
			 * @param table The table to register.
			 */
			void registerTable(const ResourceBindingTable& table);

			/**
			 * Get the descriptor set from the manager.
			 * Note that this will not create new descriptor sets.
			 *
			 * @param table the table to get the descriptor set from.
			 * @return The descriptor set.
			 */
			[[nodiscard]] VkDescriptorSet getDescriptorSet(const ResourceBindingTable& table) const;

		private:
			/**
			 * Copy the descriptor set from the source to destination.
			 *
			 * @param source The source descriptor.
			 * @param destination The destination descriptor.
			 * @param table The table to generate the copy data.
			 */
			void copyDescriptorSet(VkDescriptorSet source, VkDescriptorSet destination, const ResourceBindingTable& table) const;

		protected:
			std::vector<VkDescriptorPoolSize> m_PoolSizes;

			std::unordered_map<uint32_t, VkDescriptorType> m_DescriptorTypeMap;
			std::unordered_map<uint64_t, DescriptorSet> m_DescriptorSets;

			VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
			VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

		private:
			VulkanEngine& m_Engine;
		};
	}
}