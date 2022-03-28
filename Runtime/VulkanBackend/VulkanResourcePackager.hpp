// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ResourcePackager.hpp"
#include "VulkanResourcePackage.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanComputePipeline.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanResourcePackager final : public ResourcePackager<VulkanDevice, VulkanResourcePackage>
		{
		public:
			VulkanResourcePackager(const uint32_t setIndex, const VulkanGraphicsPipeline* pPipeline, const VkDescriptorSetLayout vLayout);
			VulkanResourcePackager(const uint32_t setIndex, const VulkanComputePipeline* pPipeline, const VkDescriptorSetLayout vLayout);
			~VulkanResourcePackager() { if (!bIsTerminated) Terminate(); }

			virtual std::shared_ptr<VulkanResourcePackage> CreatePackage() override;
			virtual void Terminate() override;

		private:
			void CreateDescriptorPool();
			std::shared_ptr<VulkanResourcePackage> CreateResourcePackage();

		private:
			std::vector<VkDescriptorPoolSize> vPoolSizes;
			std::vector<VkWriteDescriptorSet> vWrites;

			VkDescriptorPool vDescriptorPool = VK_NULL_HANDLE;
			VkDescriptorSetLayout vDescriptorSetLayout = VK_NULL_HANDLE;
			uint32_t m_DescriptorSetCount = 0;
		};
	}
}