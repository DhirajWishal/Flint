# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Main build script for Flint.
# Set the minimum required CMake version.
cmake_minimum_required(VERSION 3.22.2)

# Set the basic project description.
project(
	Flint
	VERSION 1.0.0 
	DESCRIPTION "High performance graphics engine."
)

# Lets tell CMake to add the default ALL_BUILD, ZERO_CHECK and INSTALL to a group.
# This way we can make things much more simpler for Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "PredefinedTargets")

# Set the basic third party directory variables.
# Set the Vulkan header include directory.
set(VULKAN_HEADERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Vulkan-Headers/include)

# Set the Vulkan Memory Manager header include directory.
set(VMA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/VulkanMemoryAllocator/include)

# Set the xxHash header include directory.
set(XXHASH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/xxHash)

# Set up the assimp include, library and binary data.
set(ASSIMP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/assimp/include)

# Set the required variables for assimp.
set(ASSIMP_NO_EXPORT OFF CACHE BOOL "Disable the export functionality.")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "We don't need the tests either.")

# Add the assimp library as a subdirectory.
add_subdirectory(ThirdParty/assimp)

# Set the GLFW include, library and binary data.
set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include)

# Set the required variables for GLFW.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build the examples.'")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build the tests.")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "We don't need the docs either.")

# Add the glfw library as a subdirectory.
add_subdirectory(ThirdParty/glfw)

# Set the GLM include directory.
# Since this library is a header only library, we don't have to do anything else.
set(GLM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm)

# Set the ImGui include and source files which will later be used to compile into our project(s).
set(IM_GUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui)

# Lets now add the ImGui as a target library so we can later link it to our application.
# This makes things much more easier for us later on.
add_library(
	ImGui
	STATIC 
	${IM_GUI_INCLUDE_DIR}/imgui.cpp
	${IM_GUI_INCLUDE_DIR}/imgui_demo.cpp
	${IM_GUI_INCLUDE_DIR}/imgui_draw.cpp
	${IM_GUI_INCLUDE_DIR}/imgui_tables.cpp
	${IM_GUI_INCLUDE_DIR}/imgui_widgets.cpp
)

# Set the optick include directory.
set(OPTICK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/optick/src)

# As before, lets build the optick library as another target.
add_library(
	optick
	STATIC
	${OPTICK_INCLUDE_DIR}/optick_capi.cpp
	${OPTICK_INCLUDE_DIR}/optick_core.cpp
	${OPTICK_INCLUDE_DIR}/optick_gpu.cpp
	${OPTICK_INCLUDE_DIR}/optick_gpu.d3d12.cpp
	${OPTICK_INCLUDE_DIR}/optick_gpu.vulkan.cpp
	${OPTICK_INCLUDE_DIR}/optick_message.cpp
	${OPTICK_INCLUDE_DIR}/optick_miniz.cpp
	${OPTICK_INCLUDE_DIR}/optick_serialization.cpp
	${OPTICK_INCLUDE_DIR}/optick_server.cpp
)

# Add the Vulkan headers as a target include directory.
target_include_directories(optick PUBLIC ${VULKAN_HEADERS_INCLUDE_DIR})

# Set the SPIRV-Reflect library include directory.
set(SPIRV_REFLECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SPIRV-Reflect)

# Same as ImGui, we can add this as a target library so we can link to it later.
add_library(
	SPIRV_Reflect
	STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SPIRV-Reflect/spirv_reflect.c
)

# Set the volk meta loader include directory.
set(VOLK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/volk)

# Same as before, lets make a target library for this.
add_library(
	volk
	STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/volk/volk.c
)

# Add the Vulkan headers as a target include directory.
target_include_directories(volk PUBLIC ${VULKAN_HEADERS_INCLUDE_DIR})

# Set global compile definitions.
add_compile_definitions(
	$<$<CONFIG:Debug>:FLINT_DEBUG>
	$<$<CONFIG:Debug>:USE_OPTICK>
    $<$<CONFIG:Release>:FLINT_RELEASE>

    $<$<CONFIG:RelWithDebInfo>:FLINT_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:USE_OPTICK>
    $<$<CONFIG:MinSizeRel>:FLINT_RELEASE>
	
    $<$<PLATFORM_ID:Windows>:FLINT_PLATFORM_WINDOWS>
    $<$<PLATFORM_ID:Linux>:FLINT_PLATFORM_LINUX>
    $<$<PLATFORM_ID:Darwin>:FLINT_PLATFORM_MAC>
)

# Set the central include directory.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Runtime)

# Finally, lets add our projects.
add_subdirectory(Runtime/Core)
add_subdirectory(Runtime/Engine)
add_subdirectory(Runtime/Graphics)
add_subdirectory(Runtime/GraphicsCore)
add_subdirectory(Runtime/VulkanBackend)
add_subdirectory(Sandbox)

# Set the startup project for Visual Studio
if (MSVC) 
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)
endif ()