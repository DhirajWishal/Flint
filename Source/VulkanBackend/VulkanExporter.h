// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\ObjectExporter.h"

#include "RenderTargets\VulkanScreenBoundRenderTargetS.h"
#include "VulkanCommandBufferList.h"

namespace Flint
{
	FLINT_SETUP_TYPE_EXPORTS(
		VulkanBackend::VulkanInstance,
		VulkanBackend::VulkanDevice,
		VulkanBackend::VulkanDisplay,
		VulkanBackend::VulkanBuffer,
		VulkanBackend::VulkanScreenBoundRenderTargetS,
		VulkanBackend::VulkanCommandBufferList);
}
