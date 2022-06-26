# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	FlintEngine
	VERSION 1.0.0
	DESCRIPTION "The main engine library."
)

# Add the library.
add_library(
	FlintEngine
	STATIC

	"Source/Flint.cpp"

	"Source/Utility/FrameTimer.cpp"

	"Source/ShaderFactory/ShaderBuilder.cpp"

	"Flint.hpp"

	"Utility/FrameTimer.hpp"

	"ShaderFactory/ShaderBuilder.hpp"
	"ShaderFactory/ShaderBuildError.hpp"

	"Errors/InvalidBackend.hpp"
)

# Set the include directories.
target_include_directories(
	FlintEngine 
	PUBLIC 
	
	${GLM_INCLUDE_DIR} 
)

# Add the target links.
target_link_libraries(FlintEngine VulkanBackend)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FlintEngine PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintEngine PRIVATE "/MP")	
endif ()