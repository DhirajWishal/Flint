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

	"${FLINT_INCLUDE_DIR}/Flint/Core/Errors/AssetError.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Core/Errors/BackendError.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Core/Errors/InvalidArgumentError.hpp" 
	
	"${FLINT_INCLUDE_DIR}/Flint/Core/Containers/Bitset.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Core/Containers/FlatSet.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Core/Containers/SparseArray.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Core/Containers/BinaryMap.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Core/Containers/Reactor.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Core/Containers/WorkerGroup.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Core/Containers/Synchronized.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Core/Containers/SpinMutex.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Core/Containers/Bytes.hpp"

	"${FLINT_INCLUDE_DIR}/Flint/Core/Camera/Camera.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Core/Camera/MonoCamera.hpp"

	"${FLINT_INCLUDE_DIR}/Flint/Core/EventSystem/EventSystem.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Core/EventSystem/Keyboard.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Core/EventSystem/Mouse.hpp"

	"Camera/Camera.cpp" 
	"Camera/MonoCamera.cpp"

	"Containers/Reactor.cpp"
	"Containers/Bytes.cpp"
		
	"EventSystem/EventSystem.cpp"

	"ThirdParty/stb_image.cpp"
)

# Set the include directories.
target_include_directories(
	FlintCore 
	
	PUBLIC ${OPTICK_INCLUDE_DIR} 
	PUBLIC ${GLM_INCLUDE_DIR} 
	PUBLIC ${SDL_INCLUDE_DIR} 
	PUBLIC ${STB_INCLUDE_DIR} 
)

# Add the target links.
target_link_libraries(FlintCore optick SDL2)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FlintCore PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintCore PRIVATE "/MP")	
endif ()