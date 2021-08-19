// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanImage.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan Summed-Area Variance Shadow Maps.
		 * https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-8-summed-area-variance-shadow-maps
		 */
		class VulkanSAVSM final : public VulkanOffScreenRenderTarget
		{
		public:
			VulkanSAVSM(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, UI32 threadCount = 0);

			virtual void Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget = nullptr) override final;
			virtual void Terminate() override final;

			virtual VkFramebuffer GetFrameBuffer(UI32 index) const override final;

			virtual FColor4D GetClearColor() const override final;
			virtual void SetClearColor(const FColor4D& newColor) override final;

			virtual const UI32 GetClearScreenValueCount() const override final { return 2; }
			virtual const VkClearValue* GetClearScreenValues() const override final { return pClearValues; }

		private:
			std::unique_ptr<VulkanImage> pColorImage = nullptr;
			std::unique_ptr<VulkanImage> pDepthImage = nullptr;

			VkClearValue pClearValues[2] = {};
		};
	}
}