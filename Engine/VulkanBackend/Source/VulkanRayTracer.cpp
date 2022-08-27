// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRayTracer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

#include <Optick.h>

namespace Flint
{
	namespace Backend
	{
		VulkanRayTracer::VulkanRayTracer(const std::shared_ptr<VulkanDevice>& pDevice, Camera& camera, uint32_t frameCount)
			: RayTracer(pDevice, camera, frameCount)
		{
			OPTICK_EVENT();

			// Setup the attachments.
			createAttachments();

			// Make sure to set the object as valid.
			validate();
		}

		VulkanRayTracer::~VulkanRayTracer()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanRayTracer::terminate()
		{
			OPTICK_EVENT();

			// Wait idle to finish everything we have prior to this.
			getDevice().as<VulkanDevice>()->waitIdle();

			// Destroy the attachments.
			m_pStorageAttachments.clear();

			invalidate();
		}

		void VulkanRayTracer::update()
		{
			OPTICK_EVENT();
		}

		void VulkanRayTracer::updateExtent()
		{
			OPTICK_EVENT();
		}

		Flint::Backend::VulkanRenderTargetAttachment& VulkanRayTracer::getAttachment(uint32_t index)
		{
			return *m_pStorageAttachments[index];
		}

		const Flint::Backend::VulkanRenderTargetAttachment& VulkanRayTracer::getAttachment(uint32_t index) const
		{
			return *m_pStorageAttachments[index];
		}

		void VulkanRayTracer::createAttachments()
		{
			OPTICK_EVENT();

			m_pStorageAttachments.reserve(m_FrameCount);
			for (uint32_t i = 0; i < m_FrameCount; i++)
			{
				m_pStorageAttachments.emplace_back(
					std::make_unique<VulkanStorageAttachment>(
						getDevicePointerAs<VulkanDevice>(),
						getWidth(),
						getHeight(),
						Utility::GetPixelFormat(
							Utility::FindSupportedFormat(
								getDevice().as<VulkanDevice>(),
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