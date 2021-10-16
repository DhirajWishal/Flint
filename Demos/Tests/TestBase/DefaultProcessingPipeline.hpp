// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphics/ProcessingPipeline/ProcessingPipeline.hpp"
#include "Camera.hpp"

namespace Flint
{
	/**
	 * Default processing pipeline object.
	 * This object is derived from the Processing Pipeline object and has additional features.
	 */
	class DefaultProcessingPipeline final : public ProcessingPipeline
	{
	public:
		/**
		 * Create the processing pipeline.
		 *
		 * @param pDevice The device pointer.
		 * @param frameExtent The size of the frame.
		 * @param displayTitle The title of the display.
		 * @param pipelineCount The pipeline count to use. If set to 0, the best supported is set.
		 * @param msaaCount The number of MSAA samples to use. If the MSAA count is greater than One, a color buffer is used.
		 * @param forceColorBuffer Force to use a color buffer. Default is false.
		 * @param forceDepthBuffer Whether or not to use a depth buffer. Default is false.
		 */
		DefaultProcessingPipeline(const std::shared_ptr<Device>& pDevice, const FBox2D frameExtent, const std::string& displayTitle, const UI32 pipelineCount, const MultiSampleCount msaaCount, const bool forceColorBuffer = false, const bool forceDepthBuffer = false);

		/**
		 * Update method.
		 * This method is used to update the camera.
		 *
		 * @return The time delta.
		 */
		UI64 Update();

	public:
		/**
		 * Get the camera object.
		 *
		 * @return The camera object reference.
		 */
		Camera& GetCamera() { return mCamera; }

	private:
		std::chrono::time_point<std::chrono::steady_clock> mOldTimePoint = {};

		Camera mCamera = {};
	};
}
