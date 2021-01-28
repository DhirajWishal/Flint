// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/Pipelines/VulkanGraphicsPipeline.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanGraphicsPipeline::Initialize(VulkanDevice* pDevice, VulkanRenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigest)
		{
			this->pDevice = pDevice;

			
		}

		void VulkanGraphicsPipeline::Terminate()
		{
		}
	}
}