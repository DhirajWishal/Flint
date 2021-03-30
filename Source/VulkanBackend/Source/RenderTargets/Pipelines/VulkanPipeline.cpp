// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/Pipelines/VulkanPipeline.h"
#include "VulkanBackend/RenderTargets/Pipelines/VulkanGraphicsPipeline.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/VulkanBuffer.h"
#include "VulkanBackend/VulkanUtilities.h"

namespace Flint
{
	namespace VulkanBackend
	{
		//void VulkanPipelineResource::Bind(const std::shared_ptr<Backend::CommandBuffer>& pCommandBuffer)
		//{
		//	VulkanPipeline* pvPipeline = pPipeline->Derive<VulkanGraphicsPipeline>()->Derive<VulkanPipeline>();
		//
		//	vkCmdBindDescriptorSets(
		//		*dynamic_cast<VulkanCommandBuffer*>(const_cast<Backend::CommandBuffer*>(pCommandBuffer.get())),
		//		pvPipeline->GetBindPoint(),
		//		pvPipeline->vPipelineLayout,
		//		0, 1, &vDescriptorSet, 0, VK_NULL_HANDLE);
		//}
	}
}