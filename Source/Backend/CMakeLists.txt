# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	FlintBackend
	VERSION 1.0.0
	DESCRIPTION "Backend library for Flint."
)

# Add the library.
add_library(
	FlintBackend
	STATIC

	"${FLINT_INCLUDE_DIR}/Flint/Backend/Instance.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Device.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Backend/FObject.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Window.hpp" 
	"${FLINT_INCLUDE_DIR}/Flint/Backend/RenderTarget.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/DeviceBoundObject.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Rasterizer.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/RayTracer.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/RenderTargetAttachment.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Types.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/StaticInitializer.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Program.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/RasterizingProgram.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Buffer.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Graphical.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/StaticModel.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Pipeline.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/RasterizingPipeline.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/RayTracingPipeline.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/ComputePipeline.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/VertexStorage.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/CommandBuffers.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/ShaderCode.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/PipelineCacheHandler.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/DrawEntry.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Entity.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/MeshBindingTable.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Texture2D.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/Texture.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/TextureView.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Backend/TextureSampler.hpp"

	"StaticInitializer.cpp"
	"ShaderCode.cpp"
	"PipelineCacheHandler.cpp"
	"MeshBindingTable.cpp"
	"Texture2D.cpp" 
	"Buffer.cpp"
	"CommandBuffers.cpp"
	"Device.cpp"
	"DeviceBoundObject.cpp"
	"DrawEntry.cpp"
	"Graphical.cpp"
	"RasterizingPipeline.cpp"
	"RenderTargetAttachment.cpp"
	"Window.cpp"
	"Texture.cpp"
	"TextureView.cpp"
)

# Set the include directories.
target_include_directories(
	FlintBackend 
	
	PUBLIC ${XXHASH_INCLUDE_DIR} 
	PRIVATE ${ASSIMP_INCLUDE_DIR} 
	PUBLIC ${SPDLOG_INCLUDE_DIR}
)

# Add the target links.
target_link_libraries(FlintBackend FlintCore assimp SPIRV_Reflect)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FlintBackend PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintBackend PRIVATE "/MP")	
endif ()