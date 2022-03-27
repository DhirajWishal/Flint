// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Instance.hpp"

#include <volk.h>
#include <optional>
#include <optick.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanQueue
		{
		public:
			VulkanQueue() = default;

			void Initialize(VkPhysicalDevice vPhysicalDevice, const DeviceFlags flags);
			bool IsComplete() const;

		public:
			std::optional<uint32_t> mGraphicsFamily = {};
			std::optional<uint32_t> mComputeFamily = {};
			std::optional<uint32_t> mTransferFamily = {};

			VkQueue vGraphicsQueue = VK_NULL_HANDLE;
			VkQueue vComputeQueue = VK_NULL_HANDLE;
			VkQueue vTransferQueue = VK_NULL_HANDLE;
		};
	}
}