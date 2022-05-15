# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	VulkanBackend
	VERSION 1.0.0
	DESCRIPTION "Core library for Flint."
)

# Add the library.
add_library(
	VulkanBackend
	STATIC

	"VulkanInstance.hpp"
 "Source/VulkanInstance.cpp" "VulkanMacros.hpp")

# Set the include directories.
target_include_directories(
	VulkanBackend 
	PUBLIC 

	${VULKAN_HEADERS_INCLUDE_DIR} 
	${VOLK_INCLUDE_DIR} 
	${VMA_INCLUDE_DIR} 
	${SDL_INCLUDE_DIR} 
	${SPIRV_REFLECT_INCLUDE_DIR}
	${SPDLOG_INCLUDE_DIR}
)

# Add the optick static library as a target link.
target_link_libraries(VulkanBackend FlintCore volk SDL2)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET VulkanBackend PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(VulkanBackend PRIVATE "/MP")	
endif ()