// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "ShaderDigest.h"

namespace Flint
{
	namespace Backend
	{
		struct GraphicsPipelineSpecification;
		class GraphicsPipeline;

		/**
		 * Render Target object.
		 * Render targets are the graphics space which objects are rendererd to. These are of two types,
		 * 1. Screen bound.
		 * 2. Off screen.
		 *
		 * These are also of 2 types,
		 * 1. Software based.
		 * 2. Hardware accelerated (only for the supported devices).
		 *
		 * These are handled by the individual APIs and the users are presented with the two options of screen bound
		 * and off screen.
		 */
		class RenderTarget : public BackendObject {
		public:
			RenderTarget() {}

			/**
			 * Initialize the render target.
			 *
			 * The user can set the required buffer count. If the count is set to 0, the default buffer count is set.
			 * If the buffer count it more than whats supported, the maximum supported is set.
			 *
			 * @param pDevice: The device pointer.
			 * @param extent: The extent of the render target.
			 * @param bufferCount: The buffer count of the render target.
			 */
			virtual void Initialize(Device* pDevice, const Vector2& extent, UI32 bufferCount = 0) = 0;

			/**
			 * Terminate the render target.
			 */
			virtual void Terminate() = 0;

		public:
			/**
			 * Create a new graphics pipeline.
			 * 
			 * @param shaderDigest: The shader digests the pipeline is to use.
			 * @param spec: The pipeline specification.
			 * @return The graphcis pipeline pointer.
			 */
			virtual GraphicsPipeline* CreateGraphicsPipeline(const std::vector<ShaderDigest>& shaderDigests, const GraphicsPipelineSpecification& spec) = 0;

		public:
			Vector2 GetExtent() const { return mExtent; }

		public:
			Device* GetDevice() const { return pDevice; }

		protected:
			Device* pDevice = nullptr;
			Vector2 mExtent = {};
		};

		/**
		 * Screen Bound Render Target object.
		 *
		 * This render target supports only software based ray tracing, compute and graphics.
		 */
		class ScreenBoundRenderTarget : public RenderTarget {
		public:
			ScreenBoundRenderTarget() {}
		};

		/**
		 * Off Screen Render Target object.
		 *
		 * This render target supports software and hardware based ray tracing, compute and graphics.
		 */
		class OffScreenRenderTarget : public RenderTarget {
		public:
			OffScreenRenderTarget() {}
		};
	}
}