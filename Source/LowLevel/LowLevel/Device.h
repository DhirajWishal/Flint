// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.h"

namespace Flint
{
	namespace LowLevel
	{
		class StaggingBuffer;
		class VertexBuffer;
		class IndexBuffer;
		class UniformBuffer;

		/**
		 * Device object.
		 * This object is responsible of creating, managing and destroying resources bound to a certain device.
		 */
		class Device {
		public:
			Device() {}
			~Device() {}

			/**
			 * Initialize the device.
			 *
			 * @param display: The display object which the device is bound to.
			 */
			void Initialize(const Display& display);

			/**
			 * Terminate the device.
			 */
			void Terminate();

		public:
			/**
			 * Create a new stagging buffer used to transfer data to the GPU.
			 *
			 * @param size: The size of the buffer.
			 * @return The stagging buffer object.
			 */
			StaggingBuffer CreateStaggingBuffer(UI64 size);

			/**
			 * Create a new vertex buffer.
			 *
			 * @param vertexSize: The size of a single vertex.
			 * @param vertexCount: The number of vertexes to be stored.
			 * @return The created vertex buffer.
			 */
			VertexBuffer CreateVertexBuffer(UI64 vertexSize, UI64 vertexCount);

			/**
			 * Create a new index buffer.
			 *
			 * @param indexSize: The size of a single index.
			 * @param indexCount: The number of indexes to be stored.
			 * @return The created index buffer.
			 */
			IndexBuffer CreateIndexBuffer(UI64 indexSize, UI64 indexCount);

			/**
			 * Create a new uniform buffer.
			 *
			 * @param size: The size of the buffer.
			 * @return The created uniform buffer.
			 */
			UniformBuffer CreateUniformBuffer(UI64 size);

		public:
			Interface::DeviceHandle GetHandle() const { return mHandle; }

		private:
			Interface::DeviceHandle mHandle = FLINT_NULL_HANDLE;
		};
	}
}