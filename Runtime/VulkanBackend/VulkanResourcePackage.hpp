// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ResourcePackage.hpp"
#include "VulkanDevice.hpp"
#include "VulkanBackend/VulkanResourcePackager.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanResourcePackage final : public ResourcePackage<VulkanResourcePackager, VulkanBuffer, VulkanImage, VulkanImageView, VulkanImageSampler>
		{
		public:
			explicit VulkanResourcePackage(const std::shared_ptr<VulkanResourcePackager>& pPackager, const std::vector<uint32_t>& bufferBindings, const std::vector<uint32_t>& imageBindings, const VkDescriptorSet& vSet);

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