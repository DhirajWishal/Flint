// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"
#include "VulkanWindow.hpp"

#include "VulkanRasterizingProgram.hpp"

#include "VulkanRasterizer.hpp"
#include "VulkanRayTracer.hpp"

#include "VulkanRasterizingPipeline.hpp"
#include "VulkanRayTracingPipeline.hpp"

#include "VulkanColorAttachment.hpp"
#include "VulkanDepthAttachment.hpp"
#include "VulkanStorageAttachment.hpp"

#include "VulkanVertexStorage.hpp"
#include "VulkanStaticModel.hpp"

#include "VulkanBuffer.hpp"

namespace Flint
{
	using Instance = VulkanBackend::VulkanInstance;
	using Device = VulkanBackend::VulkanDevice;
	using Window = VulkanBackend::VulkanWindow;

	using RasterizingProgram = VulkanBackend::VulkanRasterizingProgram;

	using Rasterizer = VulkanBackend::VulkanRasterizer;
	using RayTracer = VulkanBackend::VulkanRayTracer;

	using RasterizingPipeline = VulkanBackend::VulkanRasterizingPipeline;
	using RayTracingPipeline = VulkanBackend::VulkanRayTracingPipeline;

	using ColorAttachment = VulkanBackend::VulkanColorAttachment;
	using DepthAttachment = VulkanBackend::VulkanDepthAttachment;
	using StorageAttachment = VulkanBackend::VulkanStorageAttachment;

	using VertexStorage = VulkanBackend::VulkanVertexStorage;
	using StaticModel = VulkanBackend::VulkanStaticModel;

	using Buffer = VulkanBackend::VulkanBuffer;
}