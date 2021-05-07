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

		class Device : public BackendObject
		{
		public:
			Device(Instance* pInstance) : pInstance(pInstance) {}
			virtual ~Device() {}

			virtual bool CheckDisplayCompatibility(const Display* pDisplay) = 0;

			Instance* GetInstance() const { return pInstance; }

		protected:
			Instance* pInstance = nullptr;
		};

		class DeviceBoundObject : public BackendObject
		{
		public:
			DeviceBoundObject(Device* pDevice) : pDevice(pDevice) {}
			virtual ~DeviceBoundObject() {}

			Device* GetDevice() { return pDevice; }
			const Device* GetDevice() const { return pDevice; }

		protected:
			Device* pDevice = nullptr;
		};
	}
}