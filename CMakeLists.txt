# Main build script for Flint.
# Set the minimum required CMake version.
cmake_minimum_required(VERSION 3.22.3)

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
# Set up the assimp include, library and binary data.
set(ASSIMP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/assimp/include)
set(ASSIMP_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/assimp/build/lib/Release)
set(ASSIMP_BINARY assimp-vc142-mt)

# Set the GLFW include, library and binary data.
set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include)
set(GLFW_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/build/src/Release)
set(GLFW_BINARY glfw3dll)

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
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp
)

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

# Set the Vulkan header include directory.
set(VULKAN_HEADERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Vulkan-Headers/include)

# Set the Vulkan Memory Manager header include directory.
set(VMA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/VulkanMemoryAllocator/include)

# Set the xxHash header include directory.
set(XXHASH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/xxHash)

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
