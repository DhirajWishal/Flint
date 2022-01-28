"""
Copyright 2021 Dhiraj Wishal
SPDX-License-Identifier: Apache-2.0

Flint build script.
"""

import os
import sys

def runOnWindows():
    # Build premake
    print("\nBuilding Premake ...")
    os.system("cd \"ThirdParty/premake\" && call Bootstrap.bat")

    # Build Flint
    print("Building the Flint project ...")
    os.system("call \"ThirdParty/premake/bin/release/premake5\" vs2022")

    # Build glfw
    print("\nBuilding glfw ...")
    os.system("cd \"ThirdParty/glfw\" && cmake CMakeLists.txt -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF && cmake --build . --config Release")

    # Build Assimp
    print("\nBuilding Assimp ...")
    os.system("cd \"ThirdParty/Assimp\" && cmake CMakeLists.txt -DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF && cmake --build . --config Release")


def runOnLinux():
    # Build premake
    print("\nBuilding Premake ...")
    os.system("cd \"ThirdParty/premake\" && make")

    # Build Flint
    print("Building the Flint project ...")
    os.system("call \"ThirdParty/premake/bin/release/premake5\" gmake2")

    # Build glfw
    print("\nBuilding glfw ...")
    os.system("cd \"ThirdParty/glfw\" && cmake CMakeLists.txt -DBUILD_SHARED_LIBS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF && cmake --build . --config Release")
    os.system("cd \"ThirdParty/glfw/src\" && mv libglfw3.a liblibglfw3.a")

    # Build Assimp
    print("\nBuilding Assimp ...")
    os.system("cd \"ThirdParty/Assimp\" && cmake CMakeLists.txt -DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF && cmake --build . --config Release")


if sys.platform.startswith('win32'):
    runOnWindows()

elif sys.platform.startswith('linux'):
    runOnLinux()

# Build SPIRV Cross
print("\nBuilding SPIRV-Cross ...")
os.system("cd \"ThirdParty/SPIRV-Cross\" && cmake CMakeLists.txt && cmake --build . --config Release")

# Setup imgui
print("\nSetting up imgui ...")
os.system("cd \"Demos/ThirdParty/imgui\" && git checkout docking")

# Setup google benchmark
print("\nSetting up Google Benchmark ...")
os.system("cd \"ThirdParty/benchmark\" && cmake -E make_directory \"build\" && cmake -E chdir \"build\" cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release ../ && cmake --build \"build\" --config Release")

# Setup gdown
print("\nSetting up gdown ...")
os.system("cd \"ThirdParty/gdown\" && python setup.py install")

if input("\nDownload assets from the remote data store? (y/n) ").lower() == "y":
    os.system("cd Script && python DownloadAssets.py")
else:
    print("Skipping asset download stage. You can always download the zip file from: \
    https://drive.google.com/file/d/1AYOpwipLlZyBuxZoi3QsVWPhBoYuzbCF/view?usp=sharing\
     and place it under Assets/Store")
