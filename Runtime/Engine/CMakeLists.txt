# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	FlintEngine
	VERSION 1.0.0
	DESCRIPTION "Flint engine project."
)

# Set the source and header files.
file(GLOB_RECURSE FLINT_ENGINE_SOURCE_FILES *.cpp)
file(GLOB_RECURSE FLINT_ENGINE_HEADER_FILES *.hpp)

# Add the library.
add_library(
	FlintEngine
	STATIC
	${FLINT_ENGINE_SOURCE_FILES}
	${FLINT_ENGINE_HEADER_FILES}
)

# Set the include directories.
target_include_directories(FlintEngine PUBLIC ${GLM_INCLUDE_DIR} ${OPTICK_INCLUDE_DIR})

# Add the FlintGraphics static library as a target link.
target_link_libraries(FlintEngine FlintGraphics)

# Make sure to specify the C++ standard to C++20.
target_compile_features(FlintEngine PUBLIC cxx_std_20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintEngine PRIVATE "/MP")	
endif ()
