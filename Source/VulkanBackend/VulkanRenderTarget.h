// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Templates/RenderTarget.h"
#include "Attachments/VulkanRenderTargetAttachment.h"
#include "VulkanDevice.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRenderTarget final : public Templates::RenderTarget
		{
		public:
			VulkanRenderTarget(VulkanDevice* pDevice, const std::vector<VulkanRenderTargetAttachment*>& pAttachments, const FExtent2D& extent, UI32 bufferCount);

			void Terminate();

		public:
			VkRenderPass GetRenderPass() const { return vRenderPass; }
			VkFramebuffer GetCurrentFrameBuffer() { return vFrameBuffers[mCurrentFrameIndex]; }

		private:
			void CreateRenderPass(VkPipelineBindPoint vBindPoint);
			void DestroyRenderPass();

			void CreateFrameBuffer();
			void DestroyFrameBuffers();

			void InitializeSyncObjects();
			void TerminateSyncObjects();

		private:
			std::vector<VkSemaphore> vImageAvailables;
			std::vector<VkSemaphore> vRenderFinishes;
			std::vector<VkFence> vInFlightFences;
			std::vector<VkFence> vImagesInFlightFences;

			std::vector<VkFramebuffer> vFrameBuffers;

			std::vector<VulkanRenderTargetAttachment*> pAttachments;

			VulkanDevice* pDevice = nullptr;
			VkRenderPass vRenderPass = VK_NULL_HANDLE;
		};
	}
}