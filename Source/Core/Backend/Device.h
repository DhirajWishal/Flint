// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.h"
#include "Core/Maths/Vector/Vector2.h"

namespace Flint
{
	namespace Backend
	{
		class RenderTarget;
		class Buffer;

		class GraphicsPipeline;
		struct GraphicsPipelineSpecification;

		class CommandBufferManager;

		enum class BufferUsage : UI8 {
			UNDEFINED,
			VERTEX,
			INDEX,
			UNIFORM,
			STAGGING
		};

		enum class MemoryProfile : UI8 {
			TRANSFER_FRIENDLY,
			DEVICE_ONLY,
			DRAW_RESOURCE,
		};

		enum class RenderTargetType : UI8 {
			SCREEN_BOUND,
			OFF_SCREEN
		};

		/**
		 * Device object.
		 * This represents a single device object (GPU) and is responsible of creating buffers, render targets, etc...
		 */
		class Device : public BackendObject {
		public:
			Device() {}

			/**
			 * Initialize the device.
			 *
			 * @param pDisplay: The display which the object is bound to.
			 */
			virtual void Initialize(Display* pDisplay) = 0;

			/**
			 * Terminate the device.
			 */
			virtual void Terminate() = 0;

		public:
			/**
			 * Create a new render target object.
			 * 
			 * @param type: The type of the render target.
			 * @param extent: The buffer extent.
			 * @param bufferCount: The number of buffers the  render target will contain.
			 * @return The render target object pointer.
			 */
			virtual RenderTarget* CreateRenderTarget(RenderTargetType type, const Vector2& extent, UI32 bufferCount = 0) = 0;

		public:
			/**
			 * Create a new buffer object.
			 * 
			 * @param size: The size of the buffer.
			 * @param usage: The buffer usage.
			 * @param memoryProfile: The memory profile to be used.
			 */
			virtual Buffer* CreateBuffer(UI64 size, BufferUsage usage, MemoryProfile memoryProfile) = 0;

		public:
			/**
			 * Create a new command buffer manager.
			 * This manager creates and maintains command buffers.
			 * 
			 * @param count: The number of command buffers the manager contains.
			 * @return The command buffer manager object pointer.
			 */
			virtual CommandBufferManager* CreateCommandBufferManager(UI32 count) = 0;

		public:
			Display* GetDisplay() const { return pDisplay; }

		protected:
			Display* pDisplay = nullptr;
		};
	}
}