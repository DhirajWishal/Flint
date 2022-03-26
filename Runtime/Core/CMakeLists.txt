# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	FlintCore
	VERSION 1.0.0
	DESCRIPTION "Core library for Flint."
)

# Set the source and header files.
file(GLOB_RECURSE FLINT_CORE_SOURCE_FILES *.cpp)
file(GLOB_RECURSE FLINT_CORE_HEADER_FILES *.hpp)

# Add the library.
add_library(
	FlintCore
	STATIC
	${FLINT_CORE_SOURCE_FILES}
	${FLINT_CORE_HEADER_FILES}
)

# Set the include directories.
target_include_directories(FlintCore PUBLIC ${XXHASH_INCLUDE_DIR} ${OPTICK_INCLUDE_DIR})

# Add the optick static library as a target link.
target_link_libraries(FlintCore optik)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FlintCore PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintCore PRIVATE "/MP")	
endif ()
