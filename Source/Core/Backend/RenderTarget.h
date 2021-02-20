// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBufferManager.h"
#include "ShaderDigest.h"
#include "Core/Objects/WireFrame.h"
#include "Core/Types/VectorSet.h"

namespace Flint
{
	namespace Backend
	{
		struct GraphicsPipelineSpecification;
		class Pipeline;
		class GraphicsPipeline;

		typedef UI32 EntryReference;

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
			struct DrawEntry {
				WireFrame mWireFrame = {};
				std::vector<Buffer*> pUniformBuffers;
				Pipeline* pPipeline = nullptr;
			};

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
			 * @return The graphics pipeline pointer.
			 */
			virtual GraphicsPipeline* CreateGraphicsPipeline(const std::vector<ShaderDigest>& shaderDigests, const GraphicsPipelineSpecification& spec) = 0;

		public:
			Vector2 GetExtent() const { return mExtent; }

		public:
			Device* GetDevice() const { return pDevice; }

		public:
			/**
			 * Add a draw entry to the render target.
			 *
			 * @param wireFrame: The wire frame to be rendered.
			 * @param pPipeline: The pipeline which the wire frame is rendered using.
			 * @param pUniformBuffers: The uniform buffer pointers to be used. Default is 0.
			 * @return The entry reference.
			 */
			EntryReference AddDrawEntry(const WireFrame& wireFrame, Pipeline* pPipeline, std::vector<Buffer*> pUniformBuffers = {});

			/**
			 * Remove an entry from the draw queue.
			 *
			 * @param reference: The draw entry reference to be deleted.
			 */
			void RemoveDrawEntry(EntryReference reference);

		public:
			/**
			 * Prepare command buffers to be rendered.
			 * This should be done every time the draw entry list is altered.
			 *
			 * Drawing is done in the order of entry references.
			 */
			void PrepareCommandBuffers();

			/**
			 * Get the command buffer to be rendered.
			 */
			virtual CommandBuffer GetCommandBuffer() = 0;

			/**
			 * Draw the entries to render target.
			 */
			virtual void Draw(const CommandBuffer& commandBuffer) = 0;

		protected:
			virtual void Bind(CommandBuffer commandBuffer) = 0;
			virtual void UnBind(CommandBuffer commandBuffer) = 0;

		protected:
			VectorSet<EntryReference, DrawEntry> mDrawEntries;

			CommandBufferManager* pCommandBufferManager = {};
			Device* pDevice = nullptr;
			Vector2 mExtent = {};
			UI64 mBufferCount = 0;

			EntryReference mReferenceCounter = 0;
			UI32 mFrameIndex = 0;
		};

		/**
		 * Screen Bound Render Target object.
		 *
		 * This render target supports only software based ray tracing, compute and graphics.
		 */
		class ScreenBoundRenderTarget : public RenderTarget {
		public:
			ScreenBoundRenderTarget() {}

			virtual CommandBuffer GetCommandBuffer() override final;
			virtual void Draw(const CommandBuffer& commandBuffer) override final;

		protected:
			I32 mImageIndex = 0;
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