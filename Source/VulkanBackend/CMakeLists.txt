# Set the basic project information.
project(
	FlintVulkanBackend
	VERSION 1.0.0
	DESCRIPTION "Flint Vulkan backend library."
)

# Set the source and header files.
file(GLOB_RECURSE FLINT_VULKAN_BACKEND_SOURCE_FILES *.cpp)
file(GLOB_RECURSE FLINT_VULKAN_BACKEND_HEADER_FILES *.hpp)

# Add the library.
add_library(
	FlintVulkanBackend
	STATIC
	${FLINT_VULKAN_BACKEND_SOURCE_FILES}
	${FLINT_VULKAN_BACKEND_HEADER_FILES}
)

# Set the include directories.
target_include_directories(
	FlintVulkanBackend 
	PUBLIC 
	${VOLK_INCLUDE_DIR} 
	${VULKAN_INCLUDE_DIR} 
	${GLFW_INCLUDE_DIR}
	${OPTICK_INCLUDE_DIR}
	${VMA_INCLUDE_DIR}
	${SPIRV_REFLECT_INCLUDE_DIR}
)

# Add the VulkanBackend static library as a target link.
target_link_libraries(
	FlintVulkanBackend 
	FlintGraphicsCore
	glfw
	SPIRV_Reflect
)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintVulkanBackend PRIVATE "/MP")	
endif ()
