// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/ProcessingPipeline/Nodes/ProcessingNode.hpp"

namespace Flint
{
	/**
	 * Off screen pass object.
	 * This object renders object to an off screen render target.
	 */
	class OffScreenPass : public ProcessingNode
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pProcessingPipeline The processing pipeline pointer.
		 */
		OffScreenPass(ProcessingPipeline* pProcessingPipeline);

		/**
		 * Process function override.
		 *
		 * @param pCommandBuffer The command buffer pointer.
		 * @param frameIndex The frame index.
		 * @param imageIndex THe current image index.
		 */
		virtual void Process(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 frameIndex, const UI32 imageIndex) override;

	public:
		/**
		 * Get the off screen render target.
		 *
		 * @return The render target pointer.
		 */
		std::shared_ptr<OffScreenRenderTarget> GetRenderTarget() const { return pOffScreenRenderTarget; }

	private:
		/**
		 * Create the required attachments.
		 *
		 * @return The render target attachments.
		 */
		std::vector<RenderTargetAttachment> CreateAttachments() const;

	private:
		std::shared_ptr<OffScreenRenderTarget> pOffScreenRenderTarget = nullptr;
	};
}