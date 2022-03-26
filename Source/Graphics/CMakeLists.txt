# Set the basic project information.
project(
	FlintGraphics
	VERSION 1.0.0
	DESCRIPTION "Flint graphics engine."
)

# Set the source and header files.
file(GLOB FLINT_GRAPHICS_SOURCE_FILES *.cpp)
file(GLOB FLINT_GRAPHICS_HEADER_FILES *.hpp)

# Set the include directories.
target_include_directories(
	FlintGraphics 
	${GLM_INCLUDE_DIR}
	${ASSIMP_INCLUDE_DIR}
	${OPTICK_INCLUDE_DIR}
	${VOLK_INCLUDE_DIR}
	${VULKAN_HEADERS_INCLUDE_DIR}
)

# Add the library.
add_library(
	FlintGraphics
	STATIC
	${FLINT_GRAPHICS_SOURCE_FILES}
	${FLINT_GRAPHICS_HEADER_FILES}
)

# Add the VulkanBackend static library as a target link.
target_link_libraries(FlintGraphics FlintVulkanBackend ${ASSIMP_BINARY})

# Set the link directories.
target_link_directories(FlintGraphics ${ASSIMP_LIBRARY_DIR})

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintGraphics PRIVATE "/MP")	
endif ()
