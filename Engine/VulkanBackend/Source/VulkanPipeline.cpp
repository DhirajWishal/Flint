// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanPipeline.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

#include <fstream>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanPipeline::VulkanPipeline(const std::shared_ptr<VulkanDevice>& pDevice)
			: m_pDevice(pDevice)
		{
		}

		void VulkanPipeline::destroy()
		{
			// Destroy the resources.
			m_pDevice->getDeviceTable().vkDestroyPipeline(m_pDevice->getLogicalDevice(), m_Pipeline, nullptr);
			m_pDevice->getDeviceTable().vkDestroyPipelineCache(m_pDevice->getLogicalDevice(), m_PipelineCache, nullptr);
		}
	}
}