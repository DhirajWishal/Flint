// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ResourcePackager.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanResourcePackager final : public ResourcePackager
		{
		public:
			VulkanResourcePackager(const std::shared_ptr<GraphicsPipeline>& pPipeline);
			VulkanResourcePackager(const std::shared_ptr<ComputePipeline>& pPipeline);
			~VulkanResourcePackager();

		private:
			void CreateDescriptorPool();

		private:
			std::vector<VkDescriptorPoolSize> vPoolSizes;
			std::vector<VkWriteDescriptorSet> vWrites;
			std::unordered_map<ShaderResourceKey, ShaderResourceType> resources;

			VkDescriptorPool vDescriptorPool = VK_NULL_HANDLE;
			UI32 mDescriptorSetCount = 0;
		};
	}
}