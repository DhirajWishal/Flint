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
			VulkanResourcePackage(const std::shared_ptr<ResourcePackager>& pPackager, const std::vector<UI32>& bufferBindings, const std::vector<UI32>& imageBindings, const VkDescriptorSet& vSet);

			virtual void PrepareIfNecessary() override final;

		public:
			void SetDescriptorSet(const VkDescriptorSet& vSet);

		private:
			VkDescriptorSet vDescriptorSet = VK_NULL_HANDLE;
		};
	}
}