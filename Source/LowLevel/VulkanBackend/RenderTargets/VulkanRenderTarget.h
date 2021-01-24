// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Attachments/VulkanRenderTargetAttachment.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRenderTarget : public VulkanDeviceBoundObject {
		public:
			VulkanRenderTarget() {}

			virtual void Initialize(VulkanDevice* pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, UI32 bufferCount) {}

		protected:
			std::vector<VulkanRenderTargetAttachment*> pAttachments;
			Interface::RenderTargetExtent mExtent = {};
			UI32 mBufferCount = 0;
		};
	}
}