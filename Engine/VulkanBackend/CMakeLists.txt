# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	VulkanBackend
	VERSION 1.0.0
	DESCRIPTION "Vulkan graphics backend for Flint"
)

# Add the library.
add_library(
	VulkanBackend
	STATIC

	"Source/VulkanInstance.cpp"
	"Source/VulkanDevice.cpp"
	"Source/vk_mem_alloc.cpp"
	"Source/VulkanWindow.cpp" 
	"Source/VulkanCommandBuffers.cpp"
	"Source/VulkanRasterizer.cpp" 
	"Source/VulkanColorAttachment.cpp" 
	"Source/VulkanDepthAttachment.cpp" 
	"Source/VulkanRenderTargetAttachment.cpp"
	"Source/VulkanRasterizingPipeline.cpp"
	"Source/VulkanDescriptorSetManager.cpp"
	"Source/VulkanBuffer.cpp"
	"Source/VulkanRayTracer.cpp"	
	"Source/VulkanRasterizingProgram.cpp"
	"Source/VulkanStorageAttachment.cpp"
	"Source/VulkanRayTracingPipeline.cpp"
	"Source/VulkanStaticModel.cpp"
	"Source/VulkanVertexStorage.cpp"
	"Source/VulkanRasterizingDrawEntry.cpp"
	"Source/VulkanTexture2D.cpp"
	
	"VulkanInstance.hpp"
	"VulkanMacros.hpp"
	"VulkanDevice.hpp"
	"VulkanWindow.hpp" 
	"VulkanCommandBuffers.hpp"
	"VulkanRasterizer.hpp" 
	"VulkanColorAttachment.hpp" 
	"VulkanDepthAttachment.hpp"
	"VulkanRenderTargetAttachment.hpp"
	"VulkanRasterizingPipeline.hpp" 
	"VulkanDescriptorSetManager.hpp" 
	"VulkanBuffer.hpp"
	"VulkanRasterizingProgram.hpp"
	"VulkanRayTracer.hpp"
	"VulkanStorageAttachment.hpp"
	"VulkanStaticModel.hpp"
	"VulkanVertexStorage.hpp"
	"VulkanRasterizingDrawEntry.hpp"
	"VulkanTexture2D.hpp"
)

# Set the include directories.
target_include_directories(
	VulkanBackend 

	PUBLIC ${VULKAN_HEADERS_INCLUDE_DIR} 
	PUBLIC ${VOLK_INCLUDE_DIR} 
	PUBLIC ${VMA_INCLUDE_DIR} 
	PRIVATE ${SPIRV_REFLECT_INCLUDE_DIR}
)

# Add the target links.
target_link_libraries(VulkanBackend FlintCore volk)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET VulkanBackend PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(VulkanBackend PRIVATE "/MP")	
endif ()