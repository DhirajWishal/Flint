// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/RayTracer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanStorageAttachment.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan ray tracer class.
		 * This object is used to perform ray tracing.
		 */
		class VulkanRayTracer final : public Core::RayTracer<VulkanDevice, VulkanRenderTargetAttachment>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device reference.
			 * @param camera The camera reference.
			 * @param frameCount The number of frames to record.
			 */
			explicit VulkanRayTracer(VulkanDevice& device, Camera& camera, uint32_t frameCount);

			/**
			 * Destructor.
			 */
			~VulkanRayTracer() override;

			/**
			 * Update the render target.
			 * This will perform whatever operation that should be done.
			 */
			void update() override;

			/**
			 * Update the render target extent.
			 * This needs to be called if the camera's frame extents are altered.
			 */
			void updateExtent() override;

			/**
			 * Get the render target attachment at a given index.
			 *
			 * @param index The index of the attachment.
			 * @return The attachment.
			 */
			[[nodiscard]] VulkanRenderTargetAttachment& getAttachment(uint32_t index) override;

			/**
			 * Get the render target attachment at a given index.
			 *
			 * @param index The index of the attachment.
			 * @return The attachment.
			 */
			[[nodiscard]] const VulkanRenderTargetAttachment& getAttachment(uint32_t index) const override;

		private:
			/**
			 * Create the attachments.
			 */
			void createAttachments();

		private:
			std::vector<std::unique_ptr<VulkanStorageAttachment>> m_pStorageAttachments;
		};
	}
}