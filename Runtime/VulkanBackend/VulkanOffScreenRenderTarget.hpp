// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/OffScreenRenderTarget.hpp"
#include "VulkanRenderTarget.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanOffScreenRenderTarget : public OffScreenRenderTarget<VulkanDevice, VulkanImage>
		{
		public:
			explicit VulkanOffScreenRenderTarget(VulkanDevice* pDevice, const FBox2D& extent, const uint32_t bufferCount, const std::vector<typename RenderTarget<VulkanDevice, VulkanImage>::RenderTargetAttachmentT>& imageAttachments);

			virtual void Recreate(const FBox2D& extent) override;
			virtual bool PrepareNewFrame() override;
			virtual void Terminate() override;

			VkRenderPass GetRenderPass() const { return m_vRenderTarget.m_vRenderPass; }
			const VkFramebuffer GetFramebuffer() const { return m_vRenderTarget.m_vFrameBuffers[m_FrameIndex]; }

			const uint32_t GetClearScreenValueCount() const { return static_cast<uint32_t>(m_Attachments.size()); }
			const VkClearValue* GetClearScreenValues() const { return m_vClearValues.data(); }

			std::vector<VkClearValue> GetClearScreenValueVector() const { return m_vClearValues; }
			std::vector<VkImageAspectFlags> GetClearAspectFlags() const { return m_vClearAspectFlags; }

			const VkCommandBufferInheritanceInfo* GetVulkanInheritanceInfo() const;

		protected:
			VulkanRenderTarget m_vRenderTarget;

			std::vector<VkSubpassDependency> m_vDependencies{ 2 };
			std::unique_ptr<OffScreenRenderTarget> pThisRenderTarget = nullptr;
			mutable VkCommandBufferInheritanceInfo m_vInheritInfo = {};

			std::vector<VkClearValue> m_vClearValues = {};
			std::vector<VkImageAspectFlags> m_vClearAspectFlags = {};
		};
	}
}