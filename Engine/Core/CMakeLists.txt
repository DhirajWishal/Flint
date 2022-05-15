# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	FlintCore
	VERSION 1.0.0
	DESCRIPTION "Core library for Flint."
)

# Add the library.
add_library(
	FlintCore
	STATIC

	"Source/StaticEntity.cpp"
	"Source/Window.cpp"

	"Entity.hpp"
	"GhostEntity.hpp"
	"StaticEntity.hpp"
	"Instance.hpp" 
	"Engine.hpp" 
	"FObject.hpp"
	"Window.hpp" 
	"EventSummary.hpp"

	"Errors/AssetError.hpp"
	"Errors/BackendError.hpp" 
	
	"Containers/Bitset.hpp" 
)

# Set the include directories.
target_include_directories(
	FlintCore 
	PUBLIC 
	
	${XXHASH_INCLUDE_DIR} 
	${OPTICK_INCLUDE_DIR} 
	${GLM_INCLUDE_DIR} 
	${ASSIMP_INCLUDE_DIR} 
	${SDL_INCLUDE_DIR} 
)

# Add the optick static library as a target link.
target_link_libraries(FlintCore optick assimp SDL2)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FlintCore PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintCore PRIVATE "/MP")	
endif ()