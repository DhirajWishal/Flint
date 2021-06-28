// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DrawResourceBuffer.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint vertex buffer object.
		 * Vertex buffers are used to submit information to the pipeline when drawing.
		 */
		class VertexBuffer : public DrawResourceBuffer
		{
		public:
			VertexBuffer(Device& device, UI64 size, const VertexDescriptor& descriptor) : DrawResourceBuffer(device, size), mVertexDescriptor(descriptor) {}

			/**
			 * Get the vertex descriptor of the vertex buffer.
			 *
			 * @return The vertex descriptor.
			 */
			VertexDescriptor GetVertexDescriptor() const { return mVertexDescriptor; }

			/**
			 * Get the vertex stride.
			 *
			 * @return The size in bytes.
			 */
			virtual UI64 GetStride() const override final { return mVertexDescriptor.Stride(); }

		protected:
			VertexDescriptor mVertexDescriptor = {};
		};
	}
}