// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBackend/RenderTargets/VulkanRenderTarget.h"
#include "Core/Backend/ShaderDigest.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanPipeline : public VulkanDeviceBoundObject {
		public:
			VulkanPipeline() {}
			virtual ~VulkanPipeline() {}

			virtual void Initialize(VulkanDevice* pDevice, VulkanRenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigests) {}

		protected:
			void CreateDescriptorSetLayout(const std::vector<ShaderDigest>& shaderDigests);
			void CreateDescriptorPool();

		protected:
			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout vSetLayout = VK_NULL_HANDLE;
		};
	}
}