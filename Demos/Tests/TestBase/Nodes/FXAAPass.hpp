// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "OffScreenPass.hpp"

namespace Flint
{
	/**
	 * FXAA Pass object.
	 * This object is used to perform the Fast Approximate Anti Aliasing.
	 */
	class FXAAPass final : public ProcessingNode
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pProcessingPipeline The processing pipeline pointer.
		 * @param pOffScreenPass The off screen pass pointer.
		 */
		FXAAPass(ProcessingPipeline* pProcessingPipeline, const OffScreenPass* pOffScreenPass);

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

	public:
		/**
		 * Draw the UI for the FXAA pass.
		 */
		void DrawUi();

	public:
		/**
		 * Get the anti aliased image.
		 *
		 * @return The image.
		 */
		std::shared_ptr<Image> GetAntiAliasedImage() const { return pAntiAliasedImage; }

	private:
		std::shared_ptr<ComputePipeline> pComputePipeline = nullptr;
		std::shared_ptr<ResourcePackage> pResourcePackage = nullptr;
		std::shared_ptr<Image> pAntiAliasedImage = nullptr;
		std::shared_ptr<Image> pScreenSpaceReflectionImage = nullptr;

		bool bEnableFXAA = true;
	};
}
