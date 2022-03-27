// Copyright 2021-2022 Dhiraj Wishal
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
			VulkanOffScreenRenderTarget(Device* pDevice, const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments);

			virtual void Recreate(const FBox2D& extent) override;
			virtual bool PrepareNewFrame() override;
			virtual void Terminate() override;

			VkRenderPass GetRenderPass() const { return vRenderTarget.vRenderPass; }
			const VkFramebuffer GetFramebuffer() const { return vRenderTarget.vFrameBuffers[mFrameIndex]; }

			const uint32_t GetClearScreenValueCount() const { return static_cast<uint32_t>(mAttachments.size()); }
			const VkClearValue* GetClearScreenValues() const { return vClearValues.data(); }

			std::vector<VkClearValue> GetClearScreenValueVector() const { return vClearValues; }
			std::vector<VkImageAspectFlags> GetClearAspectFlags() const { return vClearAspectFlags; }

			const VkCommandBufferInheritanceInfo* GetVulkanInheritanceInfo() const;

		protected:
			VulkanRenderTarget vRenderTarget;

			std::vector<VkSubpassDependency> vDependencies{ 2 };
			std::shared_ptr<OffScreenRenderTarget> pThisRenderTarget = nullptr;
			mutable VkCommandBufferInheritanceInfo vInheritInfo = {};

			std::vector<VkClearValue> vClearValues = {};
			std::vector<VkImageAspectFlags> vClearAspectFlags = {};
		};
	}
}