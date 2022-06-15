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
	 
	"Source/Camera/Camera.cpp" 
	"Source/Camera/MonoCamera.cpp"

	"Source/StaticInitializer.cpp"
		
	"Source/EventSystem/EventSystem.cpp"

	"Source/Utility/Hasher.cpp"

	"Instance.hpp" 
	"Device.hpp" 
	"FObject.hpp"
	"Window.hpp" 
	"RenderTarget.hpp"
	"DeviceBoundObject.hpp"
	"Rasterizer.hpp"
	"RayTracer.hpp"
	"RenderTargetAttachment.hpp"
	"Types.hpp"
	"StaticInitializer.hpp"
	"Program.hpp"
	"RasterizingProgram.hpp"
	"Buffer.hpp"
	"Graphical.hpp"
	"StaticModel.hpp"
	"Pipeline.hpp"
	"RasterizingPipeline.hpp"
	"RayTracingPipeline.hpp"
	"ComputePipeline.hpp"
	
	"Errors/AssetError.hpp"
	"Errors/BackendError.hpp" 
	
	"Containers/Bitset.hpp" 
	"Containers/FlatSet.hpp" 
	"Containers/SparseArray.hpp"
	"Containers/BinaryMap.hpp" 
	"Containers/Reactor.hpp" 

	"Camera/Camera.hpp"
	"Camera/MonoCamera.hpp"

	"EventSystem/EventSystem.hpp"
	"EventSystem/Keyboard.hpp"
	"EventSystem/Mouse.hpp"

	"Utility/Hasher.hpp" 
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
	${SPDLOG_INCLUDE_DIR}
	${SPIRV_REFLECT_INCLUDE_DIR}
)

# Add the target links.
target_link_libraries(FlintCore optick assimp SDL2 SPIRV_Reflect)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FlintCore PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintCore PRIVATE "/MP")	
endif ()