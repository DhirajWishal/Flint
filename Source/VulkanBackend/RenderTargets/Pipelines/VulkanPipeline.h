// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/ShaderDigest.h"
#include "Core/Backend/Interface/Resources.h"
#include "Core/Backend/Interface/ScreenBoundRenderTarget.h"
#include "Core/Backend/Interface/OffScreenRenderTarget.h"

#include "VulkanBackend/VulkanResourceContainer.h"
#include "VulkanBackend/RenderTargets/VulkanRenderTarget.h"

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
			void ResolveUniformLayouts(const std::vector<ShaderDigest>& shaderDigests);

		protected:
			std::unordered_map<String, UniformLayout> mUniformLayouts;

			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout vSetLayout = VK_NULL_HANDLE;
		};
	}
}