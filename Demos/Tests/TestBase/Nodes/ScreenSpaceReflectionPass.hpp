// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "OffScreenPass.hpp"
#include <glm/glm.hpp>

namespace Flint
{
	/**
	 * Screen space reflection pass object.
	 * This object is used to compute the screen space reflections.
	 */
	class ScreenSpaceReflectionPass final : public ProcessingNode
	{
		struct LensProjection
		{
			glm::mat4 mMatrix = glm::mat4(1.0f);
		} *pLensData;

	public:
		/**
		 * Constructor.
		 *
		 * @param pProcessingPipeline The processing pipeline pointer.
		 * @param pOffScreenPass The off screen pass pointer.
		 */
		ScreenSpaceReflectionPass(ProcessingPipeline* pProcessingPipeline, OffScreenPass* pOffScreenPass);

		/**
		 * Process function override.
		 * This function will perform the SSR operation.
		 *
		 * @param pCommandBuffer The current command buffer pointer.
		 * @param frameIndex The current frame index.
		 * @param imageIndex The current image index.
		 */
		virtual void Process(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 frameIndex, const UI32 imageIndex) override;

	private:
		std::shared_ptr<ComputePipeline> pComputePipeline = nullptr;
		std::vector<std::shared_ptr<ResourcePackage>> pResourcePackages = {};
		std::shared_ptr<Image> pOutputImage = nullptr;
		std::shared_ptr<Buffer> pLensProjection = nullptr;

		OffScreenPass* pOffScreenPass = nullptr;
	};
}
