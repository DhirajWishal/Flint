# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	FlintFoliageEngine
	VERSION 1.0.0
	DESCRIPTION "Foliage engine. This engine reproduces a natural looking (PBR) scene, mainly using Ray Tracing."
)

# Add the library.
add_library(
	FlintFoliageEngine
	STATIC

	"Source/FoliageScene.cpp"

	"FoliageScene.hpp"
)

# Set the include directories.
target_include_directories(
	FlintFoliageEngine 
	PUBLIC 
	
	${GLM_INCLUDE_DIR} 
)

# Add the target links.
target_link_libraries(FlintFoliageEngine Engine)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FlintFoliageEngine PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintFoliageEngine PRIVATE "/MP")	
endif ()