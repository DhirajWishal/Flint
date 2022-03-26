# Set the basic project information.
project(
	FlintGraphics
	VERSION 1.0.0
	DESCRIPTION "Flint graphics engine."
)

# Set the source and header files.
file(GLOB_RECURSE FLINT_GRAPHICS_SOURCE_FILES *.cpp)
file(GLOB_RECURSE FLINT_GRAPHICS_HEADER_FILES *.hpp)

# Add the library.
add_library(
	FlintGraphics
	STATIC
	${FLINT_GRAPHICS_SOURCE_FILES}
	${FLINT_GRAPHICS_HEADER_FILES}
)

# Set the include directories.
target_include_directories(
	FlintGraphics 
	PUBLIC 
	${GLM_INCLUDE_DIR}
	${ASSIMP_INCLUDE_DIR}
	${OPTICK_INCLUDE_DIR}
	${VOLK_INCLUDE_DIR}
	${VULKAN_HEADERS_INCLUDE_DIR}
)

# Add the VulkanBackend static library as a target link.
target_link_libraries(FlintGraphics FlintVulkanBackend assimp)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintGraphics PRIVATE "/MP")	
endif ()
