-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Shader Tools project description ----------

project "ShaderTools"
	kind "StaticLib"
	language "C++"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "On"

	targetdir "$(SolutionDir)Builds/Binaries/$(Configuration)-$(Platform)"
	objdir "$(SolutionDir)Builds/Intermediate/$(Configuration)-$(Platform)/$(ProjectName)"

	files {
		"**.txt",
		"**.cpp",
		"**.h",
		"**.lua",
		"**.txt",
		"**.md",
	}

	includedirs {
		"$(SolutionDir)Source/",
		"$(SolutionDir)Source/",
		"$(SolutionDir)Dependencies/ThirdParty/SPIRV-Cross/",
		"%{IncludeDir.glslang}",
		"%{IncludeDir.shaderc}",
	}

	libdirs {
		"%{IncludeLib.glslang}",
		"%{IncludeLib.shaderc}",
	}

	links { 
		"glslang",
		"HLSL",
		"OGLCompiler",
		"OSDependent",

		"SPIRV",
		"SPIRV-Cross",
		"SPIRV-Tools",
		"SPIRV-Tools-opt",
		"SPVRemapper",

		"shaderc",
		"shaderc_combined",
		"shaderc_shared",
		"shaderc_util",
	}