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
		FXAAPass(ProcessingPipeline* pProcessingPipeline, OffScreenPass* pOffScreenPass);

		/**
		 * Process function override.
		 * This function will perform the FXAA operation.
		 *
		 * @param pCommandBuffer The current command buffer pointer.
		 * @param frameIndex The current frame index.
		 * @param imageIndex The current image index.
		 */
		virtual void Process(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 frameIndex, const UI32 imageIndex) override;

	public:
		/**
		 * Draw the UI for the FXAA pass.
		 */
		void DrawUi();

	private:
		std::shared_ptr<ComputePipeline> pComputePipeline = nullptr;
		std::shared_ptr<ResourcePackage> pResourcePackage = nullptr;
		std::shared_ptr<Image> pAntiAliasedImage = nullptr;

		OffScreenPass* pOffScreenPass = nullptr;

		bool bEnableFXAA = true;
	};
}
