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
	/**
	 * Vulkan backend exported namespace.
	 * This namespace contains all the Vulkan objects.
	 *
	 * Note that most of the objects here have additional functions, which are backend dependent. Make sure to use only the super class's functions to interact with it.
	 */
	namespace Backend
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
}