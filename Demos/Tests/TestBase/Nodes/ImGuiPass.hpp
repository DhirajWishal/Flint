// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphics/ProcessingPipeline/Nodes/ProcessingNode.hpp"
#include "TestBase/ImGuiAdapter.hpp"
#include "GraphicsCore/OffScreenRenderTarget.hpp"

namespace Flint
{
	/**
	 * ImGui Pass.
	 * This object will render the ImGui UI components to a off screen frame buffer and will copy it to the swap chain.
	 */
	class ImGuiPass final : public ProcessingNode
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pProcessingPipeline The processing pipeline pointer.
		 */
		ImGuiPass(ProcessingPipeline* pProcessingPipeline);

		/**
		 * Process function override.
		 * This function will perform the FXAA operation.
		 *
		 * @param pPreviousNode The node that was executed before this.
		 * @param pCommandBuffer The current command buffer pointer.
		 * @param frameIndex The current frame index.
		 * @param imageIndex The current image index.
		 */
		virtual void Process(ProcessingNode* pPreviousNode, const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 frameIndex, const uint32 imageIndex) override;

	private:
		/**
		 * Create the required attachments.
		 *
		 * @return The render target attachments.
		 */
		std::vector<RenderTargetAttachment> CreateAttachments() const;

	private:
		ImGuiAdapter mImGuiAdapter = {};
		std::shared_ptr<OffScreenRenderTarget> pOffScreenRenderTarget = nullptr;
		ImGuiTextureContainer mTextureContainer = {};
	};
}