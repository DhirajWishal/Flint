// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanResourcePackager.hpp"
#include "VulkanBackend/VulkanResourcePackage.hpp"
#include "VulkanBackend/VulkanShader.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanResourcePackager::VulkanResourcePackager(const UI32 setIndex, const std::shared_ptr<GraphicsPipeline>& pPipeline, const VkDescriptorSetLayout vLayout)
			: ResourcePackager(setIndex, pPipeline), vDescriptorSetLayout(vLayout)
		{
			// Resolve vertex shader data.
			{
				VulkanShader& vShader = pPipeline->GetVertexShader()->StaticCast<VulkanShader>();
				const auto resources = vShader.GetShaderResources().at(mSetIndex);
				mResources.insert(resources.begin(), resources.end());

				const auto poolSizes = vPoolSizes = vShader.GetDescriptorSetMap().at(setIndex).mPoolSizes;
				vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
			}

			// Check and resolve fragment shader data.
			if (pPipeline->GetFragmentShader())
			{
				VulkanShader& vShader = pPipeline->GetFragmentShader()->StaticCast<VulkanShader>();
				const auto resources = vShader.GetShaderResources().at(mSetIndex);
				mResources.insert(resources.begin(), resources.end());

				const auto poolSizes = vPoolSizes = vShader.GetDescriptorSetMap().at(setIndex).mPoolSizes;
				vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
			}

			// Check and resolve tessellation control shader data.
			if (pPipeline->GetTessellationControlShader())
			{
				VulkanShader& vShader = pPipeline->GetTessellationControlShader()->StaticCast<VulkanShader>();
				const auto resources = vShader.GetShaderResources().at(mSetIndex);
				mResources.insert(resources.begin(), resources.end());

				const auto poolSizes = vPoolSizes = vShader.GetDescriptorSetMap().at(setIndex).mPoolSizes;
				vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
			}

			// Check and resolve tessellation evaluation shader data.
			if (pPipeline->GetTessellationEvaluationShader())
			{
				VulkanShader& vShader = pPipeline->GetTessellationEvaluationShader()->StaticCast<VulkanShader>();
				const auto resources = vShader.GetShaderResources().at(mSetIndex);
				mResources.insert(resources.begin(), resources.end());

				const auto poolSizes = vPoolSizes = vShader.GetDescriptorSetMap().at(setIndex).mPoolSizes;
				vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
			}

			// Check and resolve geometry shader data.
			if (pPipeline->GetGeometryShader())
			{
				VulkanShader& vShader = pPipeline->GetGeometryShader()->StaticCast<VulkanShader>();
				const auto resources = vShader.GetShaderResources().at(mSetIndex);
				mResources.insert(resources.begin(), resources.end());

				const auto poolSizes = vPoolSizes = vShader.GetDescriptorSetMap().at(setIndex).mPoolSizes;
				vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
			}
		}

		VulkanResourcePackager::VulkanResourcePackager(const UI32 setIndex, const std::shared_ptr<ComputePipeline>& pPipeline, const VkDescriptorSetLayout vLayout)
			: ResourcePackager(setIndex, pPipeline), vDescriptorSetLayout(vLayout)
		{
			VulkanShader& vShader = pPipeline->GetShader()->StaticCast<VulkanShader>();
			mResources = vShader.GetShaderResources().at(mSetIndex);
			vPoolSizes = vShader.GetDescriptorSetMap().at(setIndex).mPoolSizes;
		}

		VulkanResourcePackager::~VulkanResourcePackager()
		{
			if (vDescriptorPool)
				vkDestroyDescriptorPool(pPipeline->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), vDescriptorPool, nullptr);
		}

		std::shared_ptr<ResourcePackage> VulkanResourcePackager::CreatePackage()
		{
			mDescriptorSetCount++;
			CreateDescriptorPool();
			return CreateResourcePackage();
		}

		void VulkanResourcePackager::CreateDescriptorPool()
		{
			if (vDescriptorPool)
				vkDestroyDescriptorPool(pPipeline->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), vDescriptorPool, nullptr);

			VkDescriptorPoolCreateInfo vPoolCreateInfo = {};
			vPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			vPoolCreateInfo.pNext = VK_NULL_HANDLE;
			vPoolCreateInfo.flags = 0;
			vPoolCreateInfo.maxSets = mDescriptorSetCount;
			vPoolCreateInfo.poolSizeCount = static_cast<UI32>(vPoolSizes.size());
			vPoolCreateInfo.pPoolSizes = vPoolSizes.data();

			FLINT_VK_ASSERT(vkCreateDescriptorPool(pPipeline->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), &vPoolCreateInfo, nullptr, &vDescriptorPool));
		}

		std::shared_ptr<VulkanResourcePackage> VulkanResourcePackager::CreateResourcePackage()
		{
			std::vector<UI32> buffers, images;
			for (const auto binding : mResources)
			{
				if (binding.second == ShaderResourceType::Sampler ||
					binding.second == ShaderResourceType::SampledImage ||
					binding.second == ShaderResourceType::StorageImage ||
					binding.second == ShaderResourceType::CombinedImageSampler)
					images.push_back(binding.first);
				else
					buffers.push_back(binding.first);
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

				FLINT_VK_ASSERT(vkAllocateDescriptorSets(pPipeline->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), &vAllocateInfo, vDescriptorSets.data()));
			}

			// Update the existing packages.
			for (UI64 i = 0; i < pPackages.size(); i++)
				pPackages[i]->StaticCast<VulkanResourcePackage>().SetDescriptorSet(vDescriptorSets[i]);

			// Create the new package.
			auto pNewPackage = std::make_shared<VulkanResourcePackage>(shared_from_this(), buffers, images, vDescriptorSets.back());
			pPackages.push_back(pNewPackage);

			return pNewPackage;
		}
	}
}