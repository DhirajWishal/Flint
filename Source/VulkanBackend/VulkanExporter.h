// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\ObjectExporter.h"

#include "RenderTargets\Pipelines\VulkanGraphicsPipeline.h"
#include "VulkanCommandBufferList.h"

FLINT_PACK_OBJECTS(
	Flint::VulkanBackend::VulkanInstance,
	Flint::VulkanBackend::VulkanDevice,
	Flint::VulkanBackend::VulkanDisplay,
	Flint::VulkanBackend::VulkanBuffer,
	Flint::VulkanBackend::VulkanScreenBoundRenderTargetS,
	Flint::VulkanBackend::VulkanGraphicsPipeline,
	Flint::VulkanBackend::VulkanPipelineResource,
	Flint::VulkanBackend::VulkanCommandBufferList)
