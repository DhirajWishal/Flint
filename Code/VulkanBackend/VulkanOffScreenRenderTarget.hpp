// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/OffScreenRenderTarget.hpp"
#include "VulkanRenderTarget.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanOffScreenRenderTarget : public OffScreenRenderTarget, public std::enable_shared_from_this<VulkanOffScreenRenderTarget>
		{
		public:
			VulkanOffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments);

			virtual void Terminate() override final;

			VkRenderPass GetRenderPass() const { return vRenderTarget.vRenderPass; }
			const VkFramebuffer PrepareAndGetFramebuffer();

			UI32 GetClearScreenValueCount() const { return static_cast<UI32>(mAttachments.size()); }
			std::vector<VkClearValue> GetClearScreenValues() const;

		protected:
			VulkanRenderTarget vRenderTarget;

			std::vector<VkSubpassDependency> vDependencies{ 2 };
			std::shared_ptr<OffScreenRenderTarget> pThisRenderTarget = nullptr;
			VkCommandBufferInheritanceInfo vInheritInfo = {};
		};
	}
}