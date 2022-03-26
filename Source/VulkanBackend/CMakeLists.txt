# Set the basic project information.
project(
	FlintVulkanBackend
	VERSION 1.0.0
	DESCRIPTION "Flint Vulkan backend library."
)

# Set the source and header files.
file(GLOB FLINT_VULKAN_BACKEND_SOURCE_FILES *.cpp)
file(GLOB FLINT_VULKAN_BACKEND_HEADER_FILES *.hpp)

# Set the include directories.
target_include_directories(
	FlintVulkanBackend 
	${VOLK_INCLUDE_DIR} 
	${VULKAN_INCLUDE_DIR} 
	${GLFW_INCLUDE_DIR}
	${OPTICK_INCLUDE_DIR}
	${VMA_INCLUDE_DIR}
)

# Add the library.
add_library(
	FlintVulkanBackend
	STATIC
	${FLINT_VULKAN_BACKEND_SOURCE_FILES}
	${FLINT_VULKAN_BACKEND_HEADER_FILES}
)

# Add the VulkanBackend static library as a target link.
target_link_libraries(
	FlintVulkanBackend 
	FlintGraphicsCore
	${GLFW_BINARY}
)

# Set the target link directories.
target_link_directories(FlintVulkanBackend ${GLFW_LIBRARY_DIR})

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintVulkanBackend PRIVATE "/MP")	
endif ()
