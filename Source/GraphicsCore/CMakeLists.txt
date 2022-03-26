# Set the basic project information.
project(
	FlintGraphicsCore
	VERSION 1.0.0
	DESCRIPTION "Flint graphics core library."
)

# Set the source and header files.
file(GLOB_RECURSE FLINT_GRAPHICS_CORE_SOURCE_FILES *.cpp)
file(GLOB_RECURSE FLINT_GRAPHICS_CORE_HEADER_FILES *.hpp)

# Add the library.
add_library(
	FlintGraphicsCore
	STATIC
	${FLINT_GRAPHICS_CORE_SOURCE_FILES}
	${FLINT_GRAPHICS_CORE_HEADER_FILES}
)

# Set the include directories.
target_include_directories(
	FlintGraphicsCore 
	PUBLIC 
	${VOLK_INCLUDE_DIR} 
	${VULKAN_HEADERS_INCLUDE_DIR} 
	${OPTICK_INCLUDE_DIR}
)

# Add the VulkanBackend static library as a target link.
target_link_libraries(FlintGraphicsCore VulkanBackend)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintGraphicsCore PRIVATE "/MP")	
endif ()
