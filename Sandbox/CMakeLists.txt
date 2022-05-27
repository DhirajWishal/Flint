# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	Sandbox
	VERSION 1.0.0
	DESCRIPTION "Sample application."
)

# Add the library.
add_executable(
	Sandbox

	"Main.cpp"
)

# Add the optick static library as a target link.
target_link_libraries(Sandbox FlintEngine)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET Sandbox PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(Sandbox PRIVATE "/MP")	
endif ()

# Copy the required files.
file(COPY Shaders DESTINATION ${CMAKE_BINARY_DIR}/Sandbox)

file(GLOB SDL_BUILDS_TO_COPY "${CMAKE_BINARY_DIR}/ThirdParty/SDL/*")
file(COPY ${SDL_BUILDS_TO_COPY} DESTINATION ${CMAKE_BINARY_DIR}/Sandbox)

file(GLOB ASSIMP_BUILDS_TO_COPY "${CMAKE_BINARY_DIR}/ThirdParty/assimp/bin/*")
file(COPY ${ASSIMP_BUILDS_TO_COPY} DESTINATION ${CMAKE_BINARY_DIR}/Sandbox)