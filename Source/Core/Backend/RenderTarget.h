// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBufferManager.h"
#include "ShaderDigest.h"
#include "Pipeline.h"
#include "Core/Objects/WireFrame.h"
#include "Core/Types/VectorSet.h"
#include "Core/Thread/ThreadPool.h"

namespace Flint
{
	namespace Backend
	{
		struct GraphicsPipelineSpecification;
		struct DynamicStateContainer;

		class Pipeline;
		class PipelineResource;
		class GraphicsPipeline;

		typedef UI32 EntryReference;

		enum class RenderTargetExecutionType : UI8 {
			SINGLE_THREADED,
			MULTI_THREADED,
		};

		/**
		 * Render Target object.
		 * Render targets are the graphics space which objects are rendered to. These are of two types,
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
			struct DrawEntry {
				WireFrame mWireFrame = {};
				DynamicStateContainer mDynamicStates = {};

				Pipeline* pPipeline = nullptr;
				PipelineResource* pResource = nullptr;
			};

			struct DynamicDrawEntry : public DrawEntry {
				std::unique_ptr<CommandBuffer> pCommandBuffer;
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
			 * By default, these entries are set to dynamic draw entries.
			 *
			 * @param wireFrame: The wire frame to be rendered.
			 * @param pPipeline: The pipeline which the wire frame is rendered using.
			 * @param pPipelineResource: The pipeline resources to bind.
			 * @param container: The dynamic states used by the entry.
			 * @return The entry reference.
			 */
			EntryReference AddDrawEntry(const WireFrame& wireFrame, Pipeline* pPipeline, PipelineResource* pPipelineResource, const DynamicStateContainer& container = {});

			/**
			 * Add a draw entry to the render target.
			 *
			 * @param wireFrame: The wire frame to be rendered.
			 * @param pPipeline: The pipeline which the wire frame is rendered using.
			 * @param pPipelineResource: The pipeline resources to bind.
			 * @param container: The dynamic states used by the entry.
			 * @return The entry reference.
			 */
			EntryReference AddStaticDrawEntry(const WireFrame& wireFrame, Pipeline* pPipeline, PipelineResource* pPipelineResource, const DynamicStateContainer& container = {});

			/**
			 * Remove an entry from the draw queue.
			 *
			 * @param reference: The draw entry reference to be deleted.
			 */
			void RemoveDrawEntry(EntryReference reference);

			/**
			 * Prepare the render target to draw.
			 */
			virtual UI32 PrepareToDraw() = 0;

			/**
			 * Submit commands to the GPU to be processed.
			 */
			virtual void SubmitCommand() = 0;

		public:
			/**
			 * Prepare command buffers to be rendered.
			 * This should be done every time the draw entry list is altered.
			 *
			 * Drawing is done in the order of entry references.
			 */
			void PrepareCommandBuffers();

		protected:
			virtual void Bind(const std::shared_ptr<CommandBuffer>& pCommandBuffer) = 0;
			virtual void UnBind(const std::shared_ptr<CommandBuffer>& pCommandBuffer) = 0;

			static void BakeDynamicCommands(const std::vector<RenderTarget::DrawEntry>& mEntries, const std::shared_ptr<CommandBufferManager>& pCommandBufferManager);

			void DestroyChildCommandBuffers();

		protected:
			VectorSet<EntryReference, DrawEntry> mDynamicDrawEntries;
			VectorSet<EntryReference, DrawEntry> mStaticDrawEntries;

			std::vector<std::shared_ptr<Backend::CommandBufferManager>> mChildCommandBuffers;

			Thread::ThreadPool mThreadPool = {};

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

		protected:
			UI32 mImageIndex = 0;
			UI32 mFrameIndex = 0;
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

		/** TODO
		 * Make two types of entries:
		 * * Static Entry (baked into the primary command buffer).
		 * * Dynamic Entry (baked into the secondary command buffer).
		 */
	}
}