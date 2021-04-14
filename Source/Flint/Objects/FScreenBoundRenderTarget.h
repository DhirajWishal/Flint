// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDevice.h"
#include "FDisplay.h"
#include "FGraphicsPipeline.h"

#include "Flint\Components\WireFrame.h"
#include "Core\Types\Handle.h"

namespace Flint
{
	FLINT_DEFINE_INDEX(FDrawIndex);

	/**
	 * Flint screen bound render target.
	 * Screen bound render targets output their frame to a display object.
	 * Render targets in general are the onces responsible of rendering actual draw data.
	 */
	class FScreenBoundRenderTarget final : public FObject {
	public:
		FScreenBoundRenderTarget();
		~FScreenBoundRenderTarget();

		/**
		 * Initialize the render target.
		 * 
		 * @param device: The device to which the render target is bound to.
		 * @param display: The display to which the frame is rendered to.
		 * @param bufferCount: The number of buffers the render target contains. If set to 0, the default buffer count is set. If set to std::numeric_limits<UI64>::max(), the maximum supported is set.
		 */
		void Initialize(const FDevice& device, const FDisplay& display, UI64 bufferCount);

		/**
		 * Terminate the render target.
		 */
		void Terminate();

		/**
		 * Bake draw entries to command buffers.
		 */
		void BakeCommands();

		/**
		 * Prepare resources to draw.
		 */
		void PrepareToDraw();

		/**
		 * Submit command to the device to draw.
		 */
		void SubmitCommand();

		/**
		 * Get the current frame index.
		 * 
		 * @return The index.
		 */
		UI64 GetFrameIndex() const;

		/**
		 * Get the current image index.
		 *
		 * @return The index.
		 */
		UI64 GetImageIndex() const;

	public:
		/**
		 * Add a wire frame to the static draw queue.
		 * Drawing order is done using the same order which the wire frames are submitted.
		 * Static draw entries are pre-baked into the command buffer and if your planning on changing, you need to recreate the command buffer(s)
		 * 
		 * @param wireFrame: The wire frame object to be added.
		 * @return The draw index of the submission.
		 */
		FDrawIndex AddWireFrameToStaticDrawQueue(const WireFrame& wireFrame);

		/**
		 * Remove a wire frame from the static draw queue using its index.
		 * 
		 * @param index: The index of the wire frame.
		 */
		void RemoveWireFrameFromStaticDrawQueue(const FDrawIndex index);

		/**
		 * Add a wire frame to the dynamic draw queue.
		 * Drawing order is done using the same order which the wire frames are submitted.
		 * Dynamic draw entries are baked into command buffers each iteration (tick) and can be updated before the Update() method.
		 *
		 * @param wireFrame: The wire frame object to be added.
		 * @return The draw index of the submission.
		 */
		FDrawIndex AddWireFrameToDynamicDrawQueue(const WireFrame& wireFrame);

		/**
		 * Remove a wire frame from the dynamic draw queue using its index.
		 *
		 * @param index: The index of the wire frame.
		 */
		void RemoveWireFrameFromDynamicDrawQueue(const FDrawIndex index);

	public:
		/**
		 * Create a new graphics pipeline object.
		 * 
		 * @param shaders: The shaders the pipeline uses.
		 * @param spec: The pipeline specification.
		 * @return The newly created graphics pipeline object.
		 */
		FGraphicsPipeline CreateGraphicsPipeline(const std::vector<FShader>& shaders, const Backend::GraphicsPipelineSpecification& spec);
	};
}