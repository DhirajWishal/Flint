"""
Copyright 2021 Dhiraj Wishal
SPDX-License-Identifier: Apache-2.0

Flint build script.
"""

import platform
import os

Platform = platform.system()
PremakeBinary = ""
WorkingDirectory = os.getcwd()
print("Working directory: " + WorkingDirectory)

# Platform specifics.
if Platform == "Windows":
    PremakeBinary = f"\"{WorkingDirectory}\\ThirdParty\\Binaries\\premake5\\premake5.exe\""
elif Platform == "Linux":
    PremakeBinary = "premake5"

# Build Flint
print("Premake5 binary path set to: " + PremakeBinary)
print("Building the Flint project ...")
os.system("call " + PremakeBinary + " vs2019")

# Build glfw
print("\nBuilding glfw ...")
os.system("cd \"ThirdParty/glfw\" && cmake CMakeLists.txt -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF && cmake --build . --config Release")

# Build SPIRV Cross
print("\nBuilding SPIRV-Cross ...")
os.system("cd \"ThirdParty/SPIRV-Cross\" && cmake CMakeLists.txt && cmake --build . --config Release")

# Build Assimp
print("\nBuilding Assimp ...")
os.system("cd \"Demos/ThirdParty/Assimp\" && cmake CMakeLists.txt -D BUILD_SHARED_LIBS=OFF && cmake --build . --config Debug && cmake --build . --config Release")
