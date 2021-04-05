// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDevice.h"
#include "FDisplay.h"
#include "FScreenBoundGraphicsPipeline.h"

#if defined(FLINT_BACKEND_VULKAN)
#define FLINT_SCREEN_BOUND_RT_BACKEND_SIZE		720Ui64

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	/**
	 * Flint screen bound render target.
	 * Screen bound render targets output their frame to a display object.
	 * Render targets in general are the onces responsible of rendering actual draw data.
	 */
	class FScreenBoundRenderTarget final : public FObject<FLINT_SCREEN_BOUND_RT_BACKEND_SIZE> {
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

	public:
		/**
		 * Create a new screen bound graphics pipeline object.
		 * 
		 * @param shaders: The shader digests the pipeline uses.
		 * @param spec: The pipeline specification.
		 * @return The newly created graphics pipeline object.
		 */
		FScreenBoundGraphicsPipeline CreateGraphicsPipeline(const std::vector<ShaderDigest>& shaders, const Backend::GraphicsPipelineSpecification& spec);
	};
}