// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanGraphicsPipeline.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanEngine& engine, std::string&& name, RasterizingPipelineSpecification&& specification)
			: VulkanPipeline(engine, std::move(name)), m_Specification(specification)
		{
		}

		VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
		{
		}

		void VulkanGraphicsPipeline::createPipelineLayout()
		{

		}
	}
}