# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	Sandbox
	VERSION 1.0.0
	DESCRIPTION "Sandbox project. This project is used to test out the engine and as a demo application."
)

# Set the source and header files.
file(GLOB_RECURSE FLINT_SANDBOX_SOURCE_FILES *.cpp)
file(GLOB_RECURSE FLINT_SANDBOX_HEADER_FILES *.hpp)

# Add the library.
add_executable(
	Sandbox
	${FLINT_SANDBOX_SOURCE_FILES}
	${FLINT_SANDBOX_HEADER_FILES}
)

# Add the FlintGraphics static library as a target link.
target_link_libraries(Sandbox FlintEngine)

# Make sure to specify the C++ standard to C++20.
target_compile_features(Sandbox PUBLIC cxx_std_20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(Sandbox PRIVATE "/MP")	
endif ()
