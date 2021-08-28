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

# Build shaderc
print("\nBuilding shaderc ...")
os.system("cd \"ThirdParty/shaderc\" && python utils/git-sync-deps && cmake CMakeLists.txt && cmake --build . --config Release")
os.system("cd \"ThirdParty/shaderc\" && cmake --build . --config Debug")

# Build Assimp
print("\nBuilding Assimp ...")
os.system("cd \"ThirdParty/Assimp\" && cmake CMakeLists.txt -DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF && cmake --build . --config Release")

# Setup imgui
print("\nSetting up imgui ...")
os.system("cd \"Demos/ThirdParty/imgui\" && git checkout docking")

# Setup gdown
print("\nSetting up gdown ...")
os.system("cd \"ThirdParty/gdown\" && python setup.py install")

if input("\nDownload assets from the remote data store? (y/n) ").lower() == "y":
    os.system("cd Script && python DownloadAssets.py")
else:
    print("Skipping asset download stage. You can always download the zip file from: \
    https://drive.google.com/file/d/1AYOpwipLlZyBuxZoi3QsVWPhBoYuzbCF/view?usp=sharing\
     and place it under Assets/Store")
