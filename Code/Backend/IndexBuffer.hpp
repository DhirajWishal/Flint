// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DrawResourceBuffer.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint index buffer object.
		 * Index buffers are used to submit index data to a pipeline.
		 */
		class IndexBuffer : public DrawResourceBuffer
		{
		public:
			IndexBuffer(const std::shared_ptr<Device>& pDevice, UI64 size, UI64 stride) : DrawResourceBuffer(pDevice, size), mStride(stride) {}

			/**
			 * Get the index stride.
			 *
			 * @return The size in bytes.
			 */
			virtual UI64 GetStride() const override final { return mStride; }

		protected:
			UI64 mStride = 0;
		};
	}
}