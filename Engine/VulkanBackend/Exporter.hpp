// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"
#include "VulkanWindow.hpp"
#include "VulkanRasterizer.hpp"

#include "VulkanColorAttachment.hpp"
#include "VulkanDepthAttachment.hpp"

namespace Flint
{
	namespace Backend
	{
		using Instance = VulkanBackend::VulkanInstance;
		using Device = VulkanBackend::VulkanDevice;
		using Window = VulkanBackend::VulkanWindow;
		using Rasterizer = VulkanBackend::VulkanRasterizer;

		using ColorAttachment = VulkanBackend::VulkanColorAttachment;
		using DepthAttachment = VulkanBackend::VulkanDepthAttachment;
	}
}