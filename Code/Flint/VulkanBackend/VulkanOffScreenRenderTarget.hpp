// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "OffScreenRenderTarget.hpp"
#include "VulkanRenderTarget.hpp"
#include "VulkanCommandBufferList.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanOffScreenRenderTarget final : public OffScreenRenderTarget, public std::enable_shared_from_this<VulkanOffScreenRenderTarget>
		{
		public:
			VulkanOffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, OffScreenRenderTargetAttachment attachments = OffScreenRenderTargetAttachment::COLOR_BUFFER | OffScreenRenderTargetAttachment::DEPTH_BUFFER, UI32 threadCount = 0);
		
			virtual void Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget = nullptr) override final;
			virtual void Terminate() override final;

			virtual void BindVolatileInstances() override final;
			virtual void SecondaryCommandsWorker(DrawInstanceMap& drawInstanceMap, std::list<std::shared_ptr<GeometryStore>>& drawOrder, BinarySemaphore& binarySemaphore, CountingSemaphore& countingSemaphore, std::atomic<bool>& shouldRun) override final;

			virtual FColor4D GetClearColor() const override final;
			virtual void SetClearColor(const FColor4D& newColor) override final;

			VkRenderPass GetRenderPass() const { return vRenderTarget.vRenderPass; }
			VkFramebuffer GetFrameBuffer(UI32 index) const;

			const UI32 GetClearScreenValueCount() const { return static_cast<UI32>(pResults.size()); }
			const VkClearValue* GetClearScreenValues() const { return pClearValues; }

		private:
			VulkanRenderTarget vRenderTarget;

			std::unique_ptr<VulkanCommandBufferList> pSecondaryCommandBuffer = nullptr;
			std::shared_ptr<OffScreenRenderTarget> pThisRenderTarget = nullptr;
			VkCommandBufferInheritanceInfo vInheritInfo = {};

			VkClearValue pClearValues[2] = {};
		};
	}
}