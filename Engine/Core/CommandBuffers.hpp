// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Command buffers class.
	 * This class is used to submit commands to a device to be performed.
	 *
	 * This object can contain multiple backend command buffer handles.
	 */
	class CommandBuffers : public DeviceBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device to which the commands are submitted.
		 * @param bufferCount The number of buffer primitives.
		 */
		explicit CommandBuffers(const std::shared_ptr<Device>& pDevice, uint32_t bufferCount = 1);

		/**
		 * Default virtual destructor.
		 */
		virtual ~CommandBuffers() = default;

	protected:
		const uint32_t m_BufferCount;
	};
}