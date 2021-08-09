// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "OffScreenRenderTarget.hpp"
#include "VulkanRenderTarget.hpp"

#include "VulkanColorBuffer.hpp"
#include "VulkanDepthBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanOffScreenRenderTarget final : public OffScreenRenderTarget, public std::enable_shared_from_this<VulkanOffScreenRenderTarget>
		{
		public:
			VulkanOffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList, OffScreenRenderTargetAttachment attachments = OffScreenRenderTargetAttachment::COLOR_BUFFER | OffScreenRenderTargetAttachment::DEPTH_BUFFER, UI32 threadCount = 0);
		
			virtual void Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget = nullptr) override final;
			virtual void Terminate() override final;

			std::unique_ptr<VulkanColorBuffer> pColorBuffer = nullptr;
			std::unique_ptr<VulkanDepthBuffer> pDepthBuffer = nullptr;

			VulkanRenderTarget vRenderTarget;
		};
	}
}