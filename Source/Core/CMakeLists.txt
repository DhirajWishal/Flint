# Set the basic project information.
project(
	FlintCore
	VERSION 1.0.0
	DESCRIPTION "Core library for Flint."
)

# Set the source and header files.
file(GLOB FLINT_CORE_SOURCE_FILES *.cpp)
file(GLOB FLINT_CORE_HEADER_FILES *.hpp)

# Set the include directories.
target_include_directories(FlintCore ${XXHASH_INCLUDE_DIR} ${OPTICK_INCLUDE_DIR})

# Add the library.
add_library(
	FlintCore
	STATIC
	${FLINT_CORE_SOURCE_FILES}
	${FLINT_CORE_HEADER_FILES}
)

# Add the optick static library as a target link.
target_link_libraries(FlintCore Optick)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintCore PRIVATE "/MP")	
endif ()
