// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
#include "Core\Types\DataTypes.h"

namespace Flint
{
	namespace Backend
	{
		class Display;
		class Buffer;
		class Image;

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
		 * Device object.
		 * Device objects represent GPUs and is the basis for all the resources.
		 */
		class Device : public BackendObject
		{
		public:
			/**
			 * Construct the device object.
			 *
			 * @param pInstance: The instance to which the device is bound to.
			 */
			Device(Instance* pInstance) : pInstance(pInstance) {}

			/**
			 * Check if the display is compatible with the device.
			 *
			 * @param pDisplay: The display to check.
			 * @return Boolean whether or not the display is compatible.
			 */
			virtual bool CheckDisplayCompatibility(const Display* pDisplay) = 0;

			/**
			 * Get the instance to which the device is bound to.
			 *
			 * @return The instance pointer.
			 */
			Instance* GetInstance() const { return pInstance; }

		public:
			/**
			 * Create a new buffer.
			 *
			 * @param size: The size of the buffer.
			 * @param usage: The buffer usage.
			 * @param memoryProfile: THe memory profile of the buffer.
			 *
			 * @return The buffer pointer.
			 */
			virtual Buffer* CreateBuffer(UI64 size, BufferUsage usage, MemoryProfile memoryProfile) = 0;

		protected:
			Instance* pInstance = nullptr;
		};

		/**
		 * Device bound object.
		 * This object is the base class for all the device bound objects in Flint.
		 */
		class DeviceBoundObject : public BackendObject
		{
		public:
			/**
			 * Construct the device bound object.
			 *
			 * @param pDevice: The device object pointer.
			 */
			DeviceBoundObject(Device* pDevice) : pDevice(pDevice) {}

			/**
			 * Get the device object pointer.
			 *
			 * @return The device pointer.
			 */
			Device* GetDevice() { return pDevice; }

			/**
			 * Get the device object pointer.
			 *
			 * @return The device pointer.
			 */
			const Device* GetDevice() const { return pDevice; }

		protected:
			Device* pDevice = nullptr;
		};
	}
}