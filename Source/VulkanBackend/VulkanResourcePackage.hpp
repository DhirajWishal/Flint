// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ResourcePackage.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanResourcePackage final : public ResourcePackage
		{
		public:
			VulkanResourcePackage(const std::shared_ptr<ResourcePackager>& pPackager, const std::vector<uint32>& bufferBindings, const std::vector<uint32>& imageBindings, const VkDescriptorSet& vSet);

			virtual void PrepareIfNecessary() override;

			const VkDescriptorSet GetDescriptorSet() const { return vDescriptorSet; }
			const VkDescriptorSet* GetDescriptorSetAddress() const { return &vDescriptorSet; }

		public:
			void SetDescriptorSet(const VkDescriptorSet& vSet);

		private:
			VkDescriptorSet vDescriptorSet = VK_NULL_HANDLE;
		};
	}
}