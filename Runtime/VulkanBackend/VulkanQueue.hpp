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

			void Initialize(VkPhysicalDevice m_vPhysicalDevice, const DeviceFlags flags);
			bool IsComplete() const;

		public:
			std::optional<uint32_t> m_GraphicsFamily = {};
			std::optional<uint32_t> m_ComputeFamily = {};
			std::optional<uint32_t> m_TransferFamily = {};

			VkQueue m_vGraphicsQueue = VK_NULL_HANDLE;
			VkQueue m_vComputeQueue = VK_NULL_HANDLE;
			VkQueue m_vTransferQueue = VK_NULL_HANDLE;
		};
	}
}