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
	"Source/VulkanEngine.cpp"
	"Source/vk_mem_alloc.cpp"
	"Source/VulkanWindow.cpp" 
	"Source/VulkanCommandBuffers.cpp"
	"Source/VulkanRasterizer.cpp" 
	"Source/VulkanColorAttachment.cpp" 
	"Source/VulkanDepthAttachment.cpp" 
	"Source/VulkanRenderTargetAttachment.cpp"
	"Source/VulkanGeometryStore.cpp"
	"Source/VulkanPipeline.cpp" 
	"Source/VulkanGraphicsPipeline.cpp"
	"Source/VulkanDescriptorSetManager.cpp"
	"Source/VulkanBuffer.cpp"
	"Source/VulkanBackend.cpp"
	
	"VulkanInstance.hpp"
	"VulkanMacros.hpp"
	"VulkanEngine.hpp"
	"VulkanWindow.hpp" 
	"VulkanCommandBuffers.hpp"
	"VulkanRasterizer.hpp" 
	"VulkanColorAttachment.hpp" 
	"VulkanDepthAttachment.hpp"
	"VulkanRenderTargetAttachment.hpp"
	"VulkanGeometryStore.hpp" 
	"VulkanGraphicsPipeline.hpp" 
	"VulkanPipeline.hpp"
	"VulkanDescriptorSetManager.hpp" 
	"VulkanBuffer.hpp"
	"VulkanBackend.hpp"
)

# Set the include directories.
target_include_directories(
	VulkanBackend 
	PUBLIC 

	${VULKAN_HEADERS_INCLUDE_DIR} 
	${VOLK_INCLUDE_DIR} 
	${VMA_INCLUDE_DIR} 
	${SPIRV_REFLECT_INCLUDE_DIR}
	${SPDLOG_INCLUDE_DIR}
)

# Add the target links.
target_link_libraries(VulkanBackend FlintCore volk)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET VulkanBackend PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(VulkanBackend PRIVATE "/MP")	
endif ()