// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Command buffers class.
		 * This class is used to submit commands to a device to be performed.
		 *
		 * This object can contain multiple backend command buffer handles.
		 */
		template<class TDevice>
		class CommandBuffers : public DeviceBoundObject<TDevice>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device to which the commands are submitted.
			 * @param bufferCount The number of buffer primitives.
			 */
			explicit CommandBuffers(TDevice& device, uint32_t bufferCount = 1) : DeviceBoundObject<TDevice>(device), m_BufferCount(bufferCount) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~CommandBuffers() = default;

		protected:
			const uint32_t m_BufferCount;
		};
	}
}