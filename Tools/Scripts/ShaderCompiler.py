# Copyright 2021-2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

"""
Shader compiler script.
This can be used to compile GLSL or HLSL to SPIV-V as required by Flint.

This tool exposes the 'FLINT_VERTEX_INPUT_*' and 'FLINT_INSTANCE_INPUT_*' macros.

How to use:
 $ python shaderCompiler.py -h
 $ python shaderCompiler.py --help
 $ python ShaderCompiler.py inputFile.stage 'output/outputFile.stage.spv'
 $ python ShaderCompiler.py 'input/folder' 'output/folder'
"""

import os
import sys

# Check if the user needs any help.
if len(sys.argv) < 3 or sys.argv[1] == "-h" or sys.argv[1] == "--help":
    print("Flint shader compiler utility v1.0")
    print()
    print("How to use:")
    print("$ python shaderCompiler.py -h")
    print("$ python shaderCompiler.py --help")
    print("$ python ShaderCompiler.py inputFile.stage 'output/outputFile.stage.spv'")
    print("$ python ShaderCompiler.py 'input/folder' 'output/folder'")
    print()
    print(
        "Note that if you're using this tool, you can use the 'FLINT_VERTEX_INPUT_*' and 'FLINT_INSTANCE_INPUT_*' macros."
    )
    print()
    exit(0)

# Check if we have the required environment variable.
if "VULKAN_SDK" not in os.environ:
    print("Error! Make sure that the VULKAN_SDK environment variable is set!")
    exit(-1)

# Resolve the compiler path.
compiler_path = os.path.join(os.environ["VULKAN_SDK"], "bin", "glslangValidator")

# Set the defines. More information at RasterizingProgram.hpp.
defines = ""

defines += " --D FLINT_VERTEX_INPUT_POSITION=0"
defines += " --D FLINT_VERTEX_INPUT_NORNAL=1 "
defines += " --D FLINT_VERTEX_INPUT_TANGENT=2"
defines += " --D FLINT_VERTEX_INPUT_BI_TANGENT=3"
defines += " --D FLINT_VERTEX_INPUT_COLOR_0=4"
defines += " --D FLINT_VERTEX_INPUT_COLOR_1=5"
defines += " --D FLINT_VERTEX_INPUT_COLOR_2=6"
defines += " --D FLINT_VERTEX_INPUT_COLOR_3=7"
defines += " --D FLINT_VERTEX_INPUT_COLOR_4=8"
defines += " --D FLINT_VERTEX_INPUT_COLOR_5=9"
defines += " --D FLINT_VERTEX_INPUT_COLOR_6=10"
defines += " --D FLINT_VERTEX_INPUT_COLOR_7=11"
defines += " --D FLINT_VERTEX_INPUT_TEXTURE_COORD_0=12"
defines += " --D FLINT_VERTEX_INPUT_TEXTURE_COORD_1=13"
defines += " --D FLINT_VERTEX_INPUT_TEXTURE_COORD_2=14"
defines += " --D FLINT_VERTEX_INPUT_TEXTURE_COORD_3=15"
defines += " --D FLINT_VERTEX_INPUT_TEXTURE_COORD_4=16"
defines += " --D FLINT_VERTEX_INPUT_TEXTURE_COORD_5=17"
defines += " --D FLINT_VERTEX_INPUT_TEXTURE_COORD_6=18"
defines += " --D FLINT_VERTEX_INPUT_TEXTURE_COORD_7=19"

defines += " --D FLINT_INSTANCE_INPUT_INSTANCE_ID=20"
defines += " --D FLINT_INSTANCE_INPUT_POSITION=21"
defines += " --D FLINT_INSTANCE_INPUT_ROTATION=22"
defines += " --D FLINT_INSTANCE_INPUT_SCALE=23"

# Resolve the input and output files.
input_file = sys.argv[1]
output_file = sys.argv[2]

# Compile and output if the input is a file.
if os.path.isfile(input_file):
    command = compiler_path + " -V " + input_file + " -o " + output_file

    # If the file is for a vertex shader, include the defines.s
    if ".vert" in input_file.lower():
        command += defines

    os.system(command)

# Iterate over the directory and compile the files.
elif os.path.isdir(input_file):
    print("This feature is not yet implemented! :(")
