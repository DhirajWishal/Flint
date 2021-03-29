// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#define FLINT_SETUP_TYPE_EXPORTS(...)  using Objects = Backend::ObjectTypeExporter<__VA_ARGS__>

namespace Flint
{
	namespace Backend
	{
		template<class InstanceType, class DeviceType, class DisplayType, class BufferType, class ScreenBoundRennderTargetType, class CommandBufferListType>
		class ObjectTypeExporter {
		public:
			typedef InstanceType Instance;
			constexpr Instance CreateInstanceType() const noexcept { return Instance{}; }

			typedef DeviceType Device;
			constexpr Device CreateDeviceType() const noexcept { return Device{}; }

			typedef BufferType Buffer;
			constexpr Buffer CreateBufferType() const noexcept { return Buffer{}; }

			typedef ScreenBoundRennderTargetType ScreenBoundRenderTarget;
			constexpr ScreenBoundRenderTarget CreateScreenBoundRenderTargetType() const noexcept { return ScreenBoundRenderTarget{}; }

			typedef CommandBufferListType CommandBufferList;
			constexpr CommandBufferList GetCommandBufferListType() const noexcept { return CommandBufferList{}; }

		public:
			template<class... Args>
			static Buffer CreateBuffer(Args&&... args)
			{
				Buffer buffer = {};
				buffer.Initialize(std::forward(args)...);
				return buffer;
			}
		};
	}
}