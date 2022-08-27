# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	FlintVulkanBackend
	VERSION 1.0.0
	DESCRIPTION "Vulkan graphics backend for Flint"
)

# Add the library.
add_library(
	FlintVulkanBackend
	STATIC

	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanInstance.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanMacros.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanDevice.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanWindow.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanCommandBuffers.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanRasterizer.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanColorAttachment.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanDepthAttachment.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanRenderTargetAttachment.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanRasterizingPipeline.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanDescriptorSetManager.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanBuffer.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanRasterizingProgram.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanRayTracer.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanStorageAttachment.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanStaticModel.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanVertexStorage.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanRasterizingDrawEntry.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanTexture2D.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanTextureView.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/VulkanBackend/VulkanTextureSampler.hpp"

	"VulkanInstance.cpp"
	"VulkanDevice.cpp"
	"vk_mem_alloc.cpp"
	"VulkanWindow.cpp" 
	"VulkanCommandBuffers.cpp"
	"VulkanRasterizer.cpp" 
	"VulkanColorAttachment.cpp" 
	"VulkanDepthAttachment.cpp" 
	"VulkanRenderTargetAttachment.cpp"
	"VulkanRasterizingPipeline.cpp"
	"VulkanDescriptorSetManager.cpp"
	"VulkanBuffer.cpp"
	"VulkanRayTracer.cpp"	
	"VulkanRasterizingProgram.cpp"
	"VulkanStorageAttachment.cpp"
	"VulkanRayTracingPipeline.cpp"
	"VulkanStaticModel.cpp"
	"VulkanVertexStorage.cpp"
	"VulkanRasterizingDrawEntry.cpp"
	"VulkanTexture2D.cpp"
	"VulkanTextureView.cpp"
	"VulkanTextureSampler.cpp"
)

# Set the include directories.
target_include_directories(
	FlintVulkanBackend 

	PUBLIC ${VULKAN_HEADERS_INCLUDE_DIR} 
	PUBLIC ${VOLK_INCLUDE_DIR} 
	PUBLIC ${VMA_INCLUDE_DIR} 
	PRIVATE ${SPIRV_REFLECT_INCLUDE_DIR}
)

# Add the target links.
target_link_libraries(FlintVulkanBackend FlintBackend volk SPIRV_Reflect)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FlintVulkanBackend PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintVulkanBackend PRIVATE "/MP")	
endif ()