// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.VulkanBackend.VulkanResourcePackager;

#include "GraphicsCore/ResourcePackager.hpp"
import Flint.VulkanBackend.VulkanResourcePackage;

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

module: private;

import Flint.VulkanBackend.VulkanShader;

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanResourcePackager::VulkanResourcePackager(const uint32 setIndex, const std::shared_ptr<GraphicsPipeline>& pPipeline, const VkDescriptorSetLayout vLayout)
			: ResourcePackager(setIndex, pPipeline), vDescriptorSetLayout(vLayout)
		{
			// Resolve vertex shader data.
			{
				VulkanShader& vShader = pPipeline->GetVertexShader()->StaticCast<VulkanShader>();

				const auto resourceMap = vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(mSetIndex);
					mResources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(mSetIndex).mPoolSizes;
					vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve fragment shader data.
			if (pPipeline->GetFragmentShader())
			{
				VulkanShader& vShader = pPipeline->GetFragmentShader()->StaticCast<VulkanShader>();

				const auto resourceMap = vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(mSetIndex);
					mResources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(mSetIndex).mPoolSizes;
					vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve tessellation control shader data.
			if (pPipeline->GetTessellationControlShader())
			{
				VulkanShader& vShader = pPipeline->GetTessellationControlShader()->StaticCast<VulkanShader>();

				const auto resourceMap = vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(mSetIndex);
					mResources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(mSetIndex).mPoolSizes;
					vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve tessellation evaluation shader data.
			if (pPipeline->GetTessellationEvaluationShader())
			{
				VulkanShader& vShader = pPipeline->GetTessellationEvaluationShader()->StaticCast<VulkanShader>();

				const auto resourceMap = vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(mSetIndex);
					mResources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(mSetIndex).mPoolSizes;
					vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve geometry shader data.
			if (pPipeline->GetGeometryShader())
			{
				VulkanShader& vShader = pPipeline->GetGeometryShader()->StaticCast<VulkanShader>();

				const auto resourceMap = vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(mSetIndex);
					mResources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(mSetIndex).mPoolSizes;
					vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}
		}

		VulkanResourcePackager::VulkanResourcePackager(const uint32 setIndex, const std::shared_ptr<ComputePipeline>& pPipeline, const VkDescriptorSetLayout vLayout)
			: ResourcePackager(setIndex, pPipeline), vDescriptorSetLayout(vLayout)
		{
			VulkanShader& vShader = pPipeline->GetShader()->StaticCast<VulkanShader>();

			const auto resourceMap = vShader.GetShaderResources();
			if (!resourceMap.empty())
			{
				const auto resources = resourceMap.at(mSetIndex);
				mResources.insert(resources.begin(), resources.end());
			}

			const auto descriptorSetMap = vShader.GetDescriptorSetMap();
			if (!descriptorSetMap.empty())
			{
				const auto poolSizes = descriptorSetMap.at(mSetIndex).mPoolSizes;
				vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
			}
		}

		std::shared_ptr<ResourcePackage> VulkanResourcePackager::CreatePackage()
		{
			mDescriptorSetCount++;
			CreateDescriptorPool();
			return CreateResourcePackage();
		}

		void VulkanResourcePackager::Terminate()
		{
			VulkanDevice& vDevice = pPipeline->GetDevice()->StaticCast<VulkanDevice>();

			if (vDescriptorPool)
				vDevice.GetDeviceTable().vkDestroyDescriptorPool(vDevice.GetLogicalDevice(), vDescriptorPool, nullptr);
			
			bIsTerminated = true;
		}

		void VulkanResourcePackager::CreateDescriptorPool()
		{
			VulkanDevice& vDevice = pPipeline->GetDevice()->StaticCast<VulkanDevice>();

			if (vDescriptorPool)
				vDevice.GetDeviceTable().vkDestroyDescriptorPool(vDevice.GetLogicalDevice(), vDescriptorPool, nullptr);

			VkDescriptorPoolCreateInfo vPoolCreateInfo = {};
			vPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			vPoolCreateInfo.pNext = VK_NULL_HANDLE;
			vPoolCreateInfo.flags = 0;
			vPoolCreateInfo.maxSets = mDescriptorSetCount;
			vPoolCreateInfo.poolSizeCount = static_cast<uint32>(vPoolSizes.size());
			vPoolCreateInfo.pPoolSizes = vPoolSizes.data();

			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateDescriptorPool(vDevice.GetLogicalDevice(), &vPoolCreateInfo, nullptr, &vDescriptorPool));
		}

		std::shared_ptr<VulkanResourcePackage> VulkanResourcePackager::CreateResourcePackage()
		{
			VulkanDevice& vDevice = pPipeline->GetDevice()->StaticCast<VulkanDevice>();

			std::vector<uint32> buffers, images;
			for (const auto binding : mResources)
			{
				if (binding.second == ShaderResourceType::Sampler ||
					binding.second == ShaderResourceType::SampledImage ||
					binding.second == ShaderResourceType::StorageImage ||
					binding.second == ShaderResourceType::CombinedImageSampler)
					images.emplace_back(binding.first);
				else
					buffers.emplace_back(binding.first);
			}

			std::vector<VkDescriptorSet> vDescriptorSets(mDescriptorSetCount);

			// Allocate the descriptor sets.
			{
				std::vector<VkDescriptorSetLayout> vDescriptorSetLayouts(mDescriptorSetCount, vDescriptorSetLayout);

				VkDescriptorSetAllocateInfo vAllocateInfo = {};
				vAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				vAllocateInfo.pNext = VK_NULL_HANDLE;
				vAllocateInfo.descriptorPool = vDescriptorPool;
				vAllocateInfo.descriptorSetCount = mDescriptorSetCount;
				vAllocateInfo.pSetLayouts = vDescriptorSetLayouts.data();

				FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkAllocateDescriptorSets(vDevice.GetLogicalDevice(), &vAllocateInfo, vDescriptorSets.data()));
			}

			// Update the existing packages.
			for (uint64 i = 0; i < pPackages.size(); i++)
				pPackages[i]->StaticCast<VulkanResourcePackage>().SetDescriptorSet(vDescriptorSets[i]);

			// Create the new package.
			auto pNewPackage = std::make_shared<VulkanResourcePackage>(shared_from_this(), buffers, images, vDescriptorSets.back());
			pPackages.emplace_back(pNewPackage);

			return pNewPackage;
		}
	}
}