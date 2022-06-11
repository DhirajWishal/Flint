// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRayTracer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRayTracer::VulkanRayTracer(VulkanDevice& device, Camera& camera, uint32_t frameCount)
			: Core::RayTracer<VulkanDevice, VulkanRenderTargetAttachment>(device, camera, frameCount, { Core::Defaults::ColorAttachmentDescription })
		{
			// Setup the attachments.
			createAttachments();
		}

		VulkanRayTracer::~VulkanRayTracer()
		{
			// Destroy the attachments.
			m_pStorageAttachments.clear();
		}

		void VulkanRayTracer::update()
		{

		}

		void VulkanRayTracer::updateExtent()
		{

		}

		Flint::VulkanBackend::VulkanRenderTargetAttachment& VulkanRayTracer::getAttachment(uint32_t index)
		{
			return *m_pStorageAttachments[index];
		}

		const Flint::VulkanBackend::VulkanRenderTargetAttachment& VulkanRayTracer::getAttachment(uint32_t index) const
		{
			return *m_pStorageAttachments[index];
		}

		void VulkanRayTracer::createAttachments()
		{
			m_pStorageAttachments.reserve(m_FrameCount);
			for (uint32_t i = 0; i < m_FrameCount; i++)
			{
				m_pStorageAttachments.emplace_back(
					std::make_unique<VulkanStorageAttachment>(
						getDevice(),
						getWidth(),
						getHeight(),
						Utility::GetPixelFormat(
							Utility::FindSupportedFormat(
								getDevice(),
								{ VK_FORMAT_R8G8B8A8_UNORM },
								VK_IMAGE_TILING_OPTIMAL,
								VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
								VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
								VK_FORMAT_FEATURE_TRANSFER_DST_BIT)
						))
				);
			}
		}
	}
}