// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanResourcePackager.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanResourcePackager::VulkanResourcePackager(const std::shared_ptr<GraphicsPipeline>& pPipeline)
			: ResourcePackager(pPipeline)
		{
			// Resolve vertex shader data.
			{
				VulkanShader& vVertexShader = pPipeline->GetVertexShader()->StaticCast<VulkanShader>();
				Utilities::AddPoolSizesToVector(vPoolSizes, vVertexShader);
				Utilities::AddResourcesToMap(resources, vVertexShader);
			}

			// Check and resolve fragment shader data.
			if (pPipeline->GetFragmentShader())
			{
				VulkanShader& vFragmentShader = pPipeline->GetFragmentShader()->StaticCast<VulkanShader>();
				Utilities::AddPoolSizesToVector(vPoolSizes, vFragmentShader);
				Utilities::AddResourcesToMap(resources, vFragmentShader);
			}

			// Check and resolve tessellation control shader data.
			if (pPipeline->GetTessellationControlShader())
			{
				VulkanShader& vShader = pPipeline->GetTessellationControlShader()->StaticCast<VulkanShader>();
				Utilities::AddPoolSizesToVector(vPoolSizes, vShader);
				Utilities::AddResourcesToMap(resources, vShader);
			}

			// Check and resolve tessellation evaluation shader data.
			if (pPipeline->GetTessellationEvaluationShader())
			{
				VulkanShader& vShader = pPipeline->GetTessellationEvaluationShader()->StaticCast<VulkanShader>();
				Utilities::AddPoolSizesToVector(vPoolSizes, vShader);
				Utilities::AddResourcesToMap(resources, vShader);
			}

			// Check and resolve geometry shader data.
			if (pPipeline->GetGeometryShader())
			{
				VulkanShader& vShader = pPipeline->GetGeometryShader()->StaticCast<VulkanShader>();
				Utilities::AddPoolSizesToVector(vPoolSizes, vShader);
				Utilities::AddResourcesToMap(resources, vShader);
			}
		}

		VulkanResourcePackager::VulkanResourcePackager(const std::shared_ptr<ComputePipeline>& pPipeline)
			: ResourcePackager(pPipeline)
		{
			VulkanShader& vShader = pPipeline->GetShader()->StaticCast<VulkanShader>();
			Utilities::AddPoolSizesToVector(vPoolSizes, vShader);
			Utilities::AddResourcesToMap(resources, vShader);
		}

		VulkanResourcePackager::~VulkanResourcePackager()
		{
			if (vDescriptorPool)
				vkDestroyDescriptorPool(pPipeline->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), vDescriptorPool, nullptr);
		}

		void VulkanResourcePackager::CreateDescriptorPool()
		{
			VkDescriptorPoolCreateInfo vPoolCreateInfo = {};
			vPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			vPoolCreateInfo.pNext = VK_NULL_HANDLE;
			vPoolCreateInfo.flags = 0;
			vPoolCreateInfo.maxSets = mDescriptorSetCount;
			vPoolCreateInfo.poolSizeCount = static_cast<UI32>(vPoolSizes.size());
			vPoolCreateInfo.pPoolSizes = vPoolSizes.data();

			FLINT_VK_ASSERT(vkCreateDescriptorPool(pPipeline->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), &vPoolCreateInfo, nullptr, &vDescriptorPool));
		}
	}
}