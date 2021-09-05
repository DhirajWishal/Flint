// Copyright 2021 Dhiraj Wishal
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
			VulkanResourcePackager(const UI32 setIndex, const std::shared_ptr<GraphicsPipeline>& pPipeline, const VkDescriptorSetLayout vLayout);
			VulkanResourcePackager(const UI32 setIndex, const std::shared_ptr<ComputePipeline>& pPipeline, const VkDescriptorSetLayout vLayout);
			~VulkanResourcePackager() { if (!bIsTerminated) Terminate(); }

			virtual std::shared_ptr<ResourcePackage> CreatePackage() override final;
			virtual void Terminate() override final;

		private:
			void CreateDescriptorPool();
			std::shared_ptr<VulkanResourcePackage> CreateResourcePackage();

		private:
			std::vector<VkDescriptorPoolSize> vPoolSizes;
			std::vector<VkWriteDescriptorSet> vWrites;

			VkDescriptorPool vDescriptorPool = VK_NULL_HANDLE;
			VkDescriptorSetLayout vDescriptorSetLayout = VK_NULL_HANDLE;
			UI32 mDescriptorSetCount = 0;
		};
	}
}