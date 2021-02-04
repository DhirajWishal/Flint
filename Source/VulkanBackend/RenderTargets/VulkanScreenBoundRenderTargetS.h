// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTarget.h"

#include "Attachments/VulkanColorBuffer.h"
#include "Attachments/VulkanSwapChain.h"
#include "Attachments/VulkanDepthBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Software/ compute.
		 */
		class VulkanScreenBoundRenderTargetS final : public Backend::ScreenBoundRenderTarget, public VulkanRenderTarget {
		public:
			VulkanScreenBoundRenderTargetS() {}
			~VulkanScreenBoundRenderTargetS() {}

			virtual void Initialize(Backend::Device* pDevice, const Vector2& extent, UI32 bufferCount) override final;
			virtual void Terminate() override final;

			void PrepareComputeResources();
			void PrepareRenderResources();

		private:
			VulkanSwapChain vSwapChain = {};
			VulkanColorBuffer vColorBuffer = {};
			VulkanDepthBuffer vDepthBuffer = {};

			VkDescriptorSetLayout vComputeLayout = VK_NULL_HANDLE;
			VkPipeline vComputePipeline = VK_NULL_HANDLE;
			VkPipelineLayout vComputePipelineLayout = VK_NULL_HANDLE;

			VkDescriptorSetLayout vRenderLayout = VK_NULL_HANDLE;
			VkPipeline vRenderPipeline = VK_NULL_HANDLE;
			VkPipelineLayout vRenderPipelineLayout = VK_NULL_HANDLE;
		};
	}
}