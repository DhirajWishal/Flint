// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FInstance.h"

namespace Flint
{
	class FDisplay;
	class FBuffer;

	/**
	 * Buffer usage enum.
	 * Defines what the buffer will be used for,
	 */
	enum class BufferUsage : UI8 {
		UNDEFINED,
		VERTEX, INDEX, STAGGING, UNIFORM
	};

	/**
	 * Memory profile enum.
	 * Defines what the buffer memory should look like. This can be used to optimize performance.
	 */
	enum class MemoryProfile : UI8 {
		/**
		 * This type is a must if the buffer's content is being copied at any moment.
		 * Try not to set this for buffer types like Vertex and Index as it might be costly.
		 */
		TRANSFER_FRIENDLY,

		/**
		 * This is the best for speed.
		 * Buffers with this profile has the fastest access time. Usually this is set for draw-based buffers like Vertex and Index buffers.
		 * Unlike DRAW_RESOURCE type, this has a slight performance cost.
		 */
		 DEVICE_ONLY,

		 /**
		  * This is used for buffer types like Vertex and Index buffers and is optimized for speed.
		  */
		  DRAW_RESOURCE
	};

	/**
	 * Flint device object.
	 * Device objects are the basis of all the assets. This resembles a single (or multiple in the future) GPU(s).
	 */
	class FDevice : public BackendObject {
	public:
		FDevice(std::shared_ptr<FInstance> pInstance) : pInstance(pInstance) {}
		virtual ~FDevice() {}

		/**
		 * Check if the device and display is compatible.
		 *
		 * @return Boolean value stating the validity.
		 */
		virtual bool CheckDisplayCompatibility(FDisplay* pDisplay) = 0;

		/**
		 * Get the instance pointer.
		 *
		 * @return The instance object pointer.
		 */
		std::shared_ptr<FInstance> GetInstance() const { return pInstance; }

	public:
		/**
		 * Create a new buffer object.
		 *
		 * @param size: The size of the buffer.
		 * @param usage: The buffer usage.
		 * @param memoryProfile: The buffer's memory profile.
		 * @return The created buffer pointer.
		 */
		virtual std::shared_ptr<FBuffer> CreateBuffer(UI64 size, BufferUsage usage, MemoryProfile memoryProfile) = 0;

	protected:
		std::shared_ptr<FInstance> pInstance = nullptr;
	};
}