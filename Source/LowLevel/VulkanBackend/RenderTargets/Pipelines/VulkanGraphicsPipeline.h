// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanPipeline.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanGraphicsPipeline final : public VulkanPipeline {
		public:
			VulkanGraphicsPipeline() {}
			~VulkanGraphicsPipeline() {}

			virtual void Initialize(VulkanDevice* pDevice, VulkanRenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigest);
			virtual void Terminate();
		};
	}
}