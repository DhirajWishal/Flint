// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/RenderTarget.h"
#include "Attachments/VulkanRenderTargetAttachment.h"
#include "VulkanBackend\VulkanCommandBufferManager.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRenderTarget {
		public:
			VulkanRenderTarget() {}

			virtual void Terminate() = 0;

		public:
			VkRenderPass GetRenderPass() const { return vRenderPass; }
			virtual VkFramebuffer GetCurrentFrameBuffer() const = 0;

		protected:
			void CreateRenderPass(std::vector<VulkanRenderTargetAttachment*> pAttachments, VkPipelineBindPoint vBindPoint);
			void DestroyRenderPass();

			void CreateFrameBuffer(std::vector<VulkanRenderTargetAttachment*> pAttachments, const Vector2& extent, UI32 bufferCount);
			void DestroyFrameBuffers();

		protected:
			void InitializeSyncObjects(UI32 count);
			void TerminateSyncObjects();

		protected:
			VulkanCommandBufferManager mCommandBufferManager = {};
			UI64 mBufferCount = 0;
			VulkanDevice* pDevice = nullptr;
			Vector2 mExtent = Vector2::ZeroAll;

			std::vector<VkSemaphore> vImageAvailables;
			std::vector<VkSemaphore> vRenderFinishes;
			std::vector<VkFence> vInFlightFences;
			std::vector<VkFence> vImagesInFlightFences;

			std::vector<VkFramebuffer> vFrameBuffers;
			VkRenderPass vRenderPass = VK_NULL_HANDLE;

			UI32 mFrameIndex = 0;
			UI32 mImageIndex = 0;
		};

		void DestroyRenderTarget(Backend::RenderTargetHandle handle);
	}
}