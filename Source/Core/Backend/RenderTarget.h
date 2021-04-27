// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.h"
#include "CommandBufferList.h"

namespace Flint
{
	/**
	 * Flint render target object.
	 * Render targets are the objects which performs the rendering. These are of two main types,
	 * 1. Screen bound render targets.
	 * 2. Off screen render targets.
	 */
	class FRenderTarget : public BackendObject {
		/**
		 * Draw entry structure.
		 * This object contains information about a single draw entry.
		 */
		struct DrawEntry {
			DrawEntry() = default;
			DrawEntry(FBuffer* pVertexBuffer, FBuffer* pIndexBuffer) : pVertexBuffer(pVertexBuffer), pIndexBuffer(pIndexBuffer) {}

			std::unordered_map<UI64, FPipeline*> pPipelines = {};

			FBuffer* pVertexBuffer = nullptr;
			FBuffer* pIndexBuffer = nullptr;

			UI64 mIndex = 0;
		};

	public:
		FRenderTarget() {}
		virtual ~FRenderTarget() {}

		/**
		 * Terminate the render target.
		 */
		virtual void Terminate() = 0;

		/**
		 * Bake draw entries to command buffers in the command list.
		 */
		virtual void BakeCommands() = 0;

		/**
		 * Prepare the resource for the new frame.
		 */
		virtual void PrepareToDraw() = 0;

		/**
		 * Update internals to draw.
		 */
		virtual void Update() = 0;

		/**
		 * Submit commands to the device.
		 * This is the call that actually draws data.
		 */
		virtual void SubmitCommand() = 0;

		/**
		 * Get the device which the render target is bound to.
		 *
		 * @return The device pointer.
		 */
		FDevice* GetDevice() const { return pDevice; }

		/**
		 * Get the render target extent.
		 *
		 * @return The extent.
		 */
		Vector2 GetExtent() const { return mExtent; }

		/**
		 * Get the render target buffer count.
		 *
		 * @return The buffer count.
		 */
		UI64 GetBufferCount() const { return mBufferCount; }

	public:
		/**
		 * Add a static draw entry.
		 *
		 * @param pVertexBuffer: The vertex buffer pointer.
		 * @param pIndexBuffer: The index buffer pointer.
		 * @return The entry ID.
		 */
		UI64 AddStaticDrawEntry(FBuffer* pVertexBuffer, FBuffer* pIndexBuffer);

		/**
		 * Remove a static draw entry from the render target.
		 *
		 * @param index: The index of the entry.
		 */
		void RemoveStaticDrawEntry(const UI64 index);

		/**
		 * Add a dynamic draw entry.
		 *
		 * @param pVertexBuffer: The vertex buffer pointer.
		 * @param pIndexBuffer: The index buffer pointer.
		 * @return The entry ID.
		 */
		UI64 AddDynamicDrawEntry(FBuffer* pVertexBuffer, FBuffer* pIndexBuffer);

		/**
		 * Remove a dynamic draw entry from the render target.
		 *
		 * @param index: The index of the entry.
		 */
		void RemoveDynamicDrawEntry(const UI64 index);

		/**
		 * Add a pipeline to a static draw entry.
		 *
		 * @param ID: The entry ID.
		 * @param pPipeline: The pipeline pointer.
		 * @return The pipeline ID.
		 */
		UI64 AddPipelineToStaticDrawEntry(UI64 ID, FPipeline* pPipeline);

		/**
		 * Add a pipeline to a dynamic draw entry.
		 *
		 * @param ID: The entry ID.
		 * @param pPipeline: The pipeline pointer.
		 * @return The pipeline ID.
		 */
		UI64 AddPipelineToDynamicDrawEntry(UI64 ID, FPipeline* pPipeline);

		/**
		 * Get the static draw entries of the render target.
		 *
		 * @return The entry map.
		 */
		std::unordered_map<UI64, DrawEntry>& GetStaticDrawEntries() { return mStaticDrawEntries; }

		/**
		 * Get the static draw entries of the render target.
		 *
		 * @return The entry map.
		 */
		const std::unordered_map<UI64, DrawEntry> GetStaticDrawEntries() const { return mStaticDrawEntries; }


		/**
		 * Get the dynamic draw entries of the render target.
		 *
		 * @return The entry map.
		 */
		std::unordered_map<UI64, DrawEntry>& GetDynamicDrawEntries() { return mDynamicDrawEntries; }

		/**
		 * Get the dynamic draw entries of the render target.
		 *
		 * @return The entry map.
		 */
		const std::unordered_map<UI64, DrawEntry> GetDynamicDrawEntries() const { return mDynamicDrawEntries; }

	protected:
		std::unordered_map<UI64, DrawEntry> mStaticDrawEntries;
		std::unordered_map<UI64, DrawEntry> mDynamicDrawEntries;

		FCommandBufferList* pCommandBufferList = {};

		UI64 mStaticDrawIndex = 0;
		UI64 mDynamicDrawIndex = 0;

		FDevice* pDevice = nullptr;
		Vector2 mExtent = Vector2::Zero;
		UI64 mBufferCount = 0;
	};
}