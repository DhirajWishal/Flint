// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ResourcePackager.hpp"
#include "VulkanResourcePackage.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanResourcePackager final : public ResourcePackager, public std::enable_shared_from_this<VulkanResourcePackager>
		{
		public:
			VulkanResourcePackager(const uint32 setIndex, const std::shared_ptr<GraphicsPipeline>& pPipeline, const VkDescriptorSetLayout vLayout);
			VulkanResourcePackager(const uint32 setIndex, const std::shared_ptr<ComputePipeline>& pPipeline, const VkDescriptorSetLayout vLayout);
			~VulkanResourcePackager() { if (!bIsTerminated) Terminate(); }

			virtual std::shared_ptr<ResourcePackage> CreatePackage() override;
			virtual void Terminate() override;

		private:
			void CreateDescriptorPool();
			std::shared_ptr<VulkanResourcePackage> CreateResourcePackage();

		private:
			std::vector<VkDescriptorPoolSize> vPoolSizes;
			std::vector<VkWriteDescriptorSet> vWrites;

			VkDescriptorPool vDescriptorPool = VK_NULL_HANDLE;
			VkDescriptorSetLayout vDescriptorSetLayout = VK_NULL_HANDLE;
			uint32 mDescriptorSetCount = 0;
		};
	}
}