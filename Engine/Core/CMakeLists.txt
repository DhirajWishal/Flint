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

	"Source/Containers/Reactor.cpp"

	"Source/StaticInitializer.cpp"
	"Source/ShaderCode.cpp"
	"Source/PipelineCacheHandler.cpp"
	"Source/MeshBindingTable.cpp"
	"Source/Texture2D.cpp" 
	"Source/Buffer.cpp"
	"Source/CommandBuffers.cpp"
	"Source/Device.cpp"
	"Source/DeviceBoundObject.cpp"
	"Source/DrawEntry.cpp"
	"Source/Graphical.cpp"
	"Source/RasterizingPipeline.cpp"
	"Source/RenderTargetAttachment.cpp"
	"Source/Window.cpp"
		
	"Source/EventSystem/EventSystem.cpp"

	"Source/ThirdParty/stb_image.cpp"

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
	"VertexStorage.hpp"
	"CommandBuffers.hpp"
	"ShaderCode.hpp"
	"PipelineCacheHandler.hpp"
	"DrawEntry.hpp"
	"Entity.hpp"
	"MeshBindingTable.hpp"
	"Texture2D.hpp"
	
	"Errors/AssetError.hpp"
	"Errors/BackendError.hpp" 
	"Errors/InvalidArgumentError.hpp" 
	
	"Containers/Bitset.hpp" 
	"Containers/FlatSet.hpp" 
	"Containers/SparseArray.hpp"
	"Containers/BinaryMap.hpp" 
	"Containers/Reactor.hpp" 
	"Containers/WorkerGroup.hpp" 

	"Camera/Camera.hpp"
	"Camera/MonoCamera.hpp"

	"EventSystem/EventSystem.hpp"
	"EventSystem/Keyboard.hpp"
	"EventSystem/Mouse.hpp"
)

# Set the include directories.
target_include_directories(
	FlintCore 
	
	PUBLIC ${XXHASH_INCLUDE_DIR} 
	PUBLIC ${OPTICK_INCLUDE_DIR} 
	PUBLIC ${GLM_INCLUDE_DIR} 
	PRIVATE ${ASSIMP_INCLUDE_DIR} 
	PRIVATE ${SDL_INCLUDE_DIR} 
	PUBLIC ${SPDLOG_INCLUDE_DIR}
	PRIVATE ${STB_INCLUDE_DIR} 
)

# Add the target links.
target_link_libraries(FlintCore optick assimp SDL2 SPIRV_Reflect)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FlintCore PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintCore PRIVATE "/MP")	
endif ()