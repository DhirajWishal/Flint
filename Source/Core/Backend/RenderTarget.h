// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Display.h"
#include "CommandBufferList.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint render target object.
		 * Render targets are the objects which performs the rendering. These are of two main types,
		 * 1. Screen bound render targets.
		 * 2. Off screen render targets.
		 */
		class RenderTarget : public BackendObject
		{
			/**
			 * Draw entry structure.
			 * This object contains information about a single draw entry.
			 */
			struct DrawEntry {
				DrawEntry() = default;
				DrawEntry(Buffer* pVertexBuffer, Buffer* pIndexBuffer) : pVertexBuffer(pVertexBuffer), pIndexBuffer(pIndexBuffer) {}

				std::unordered_map<UI64, Pipeline*> pPipelines = {};

				Buffer* pVertexBuffer = {};
				Buffer* pIndexBuffer = {};

				UI64 mIndex = 0;
			};

		public:
			RenderTarget(Device* pDevice, UI64 bufferCount) : pDevice(pDevice), mBufferCount(bufferCount) {}
			virtual ~RenderTarget() {}

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

			/**
			 * Get the device object of the render target.
			 *
			 * @return The device pointer.
			 */
			Device* GetDevice() const { return pDevice; }

		public:
			/**
			 * Add a static draw entry.
			 *
			 * @param pVertexBuffer: The vertex buffer pointer.
			 * @param pIndexBuffer: The index buffer pointer.
			 * @return The entry ID.
			 */
			UI64 AddStaticDrawEntry(Buffer* pVertexBuffer, Buffer* pIndexBuffer);

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
			UI64 AddDynamicDrawEntry(Buffer* pVertexBuffer, Buffer* pIndexBuffer);

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
			UI64 AddPipelineToStaticDrawEntry(UI64 ID, Pipeline* pPipeline);

			/**
			 * Add a pipeline to a dynamic draw entry.
			 *
			 * @param ID: The entry ID.
			 * @param pPipeline: The pipeline pointer.
			 * @return The pipeline ID.
			 */
			UI64 AddPipelineToDynamicDrawEntry(UI64 ID, Pipeline* pPipeline);

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

			std::unique_ptr<CommandBufferList> pCommandBufferList = nullptr;
			Device* pDevice = nullptr;

			UI64 mStaticDrawIndex = 0;
			UI64 mDynamicDrawIndex = 0;

			Vector2 mExtent = Vector2::Zero;
			UI64 mBufferCount = 0;
		};
	}
}