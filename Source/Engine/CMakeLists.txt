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

	"${FLINT_INCLUDE_DIR}/Flint/Engine/Flint.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Engine/AssetRegistry.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Engine/StaticStorage.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Engine/Scene.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Engine/SceneView.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Engine/Display.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Engine/ExecutionQueue.hpp"

	"${FLINT_INCLUDE_DIR}/Flint/Engine/Utility/FrameTimer.hpp"

	"${FLINT_INCLUDE_DIR}/Flint/Engine/ShaderFactory/ShaderBuilder.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Engine/ShaderFactory/ShaderBuildError.hpp"

	"${FLINT_INCLUDE_DIR}/Flint/Engine/Packager/Package.hpp"
	"${FLINT_INCLUDE_DIR}/Flint/Engine/Packager/PackageEntry.hpp"

	"${FLINT_INCLUDE_DIR}/Flint/Engine/Errors/InvalidBackend.hpp"

	"Flint.cpp"
	"ExecutionQueue.cpp"

	"Utility/FrameTimer.cpp"

	"ShaderFactory/ShaderBuilder.cpp"

	"Packager/Package.cpp"
)

# Add the target links.
target_link_libraries(FlintEngine FlintVulkanBackend)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FlintEngine PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FlintEngine PRIVATE "/MP")	
endif ()