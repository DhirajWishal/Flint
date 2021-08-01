-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint project description ----------

project "Flint"
	language "C++"
	kind "StaticLib"
	systemversion "latest"
	cppdialect "C++20"
	staticruntime "On"

	targetdir "$(SolutionDir)Builds/Binaries/$(Configuration)"
	objdir "$(SolutionDir)Builds/Intermediate/$(Configuration)"

	files {
		"**.txt",
		"**.cpp",
		"**.hpp",
		"**.lua",
		"**.txt",
		"**.md",
	}

	includedirs {
		"$(SolutionDir)Code/Engine",
		"$(SolutionDir)Code/Backend",
		"$(SolutionDir)Dependencies/ThirdParty/SPIRV-Cross/",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.FreeImage}",
		"%{IncludeDir.glslang}",
		"%{IncludeDir.shaderc}",
	}

	libdirs {
		"%{IncludeLib.Assimp}",
		"%{IncludeLib.FreeImage}",
		"%{IncludeLib.GLFW}",
		"%{IncludeLib.Vulkan}",
		"%{IncludeLib.glslang}",
		"%{IncludeLib.shaderc}",
	}

	links { 
		"assimp-vc142-mt",

		"glfw3dll",
		"vulkan-1",

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

	filter "configurations:Debug"
		links { 
			--"FreeImageLibd"
		}

	filter "configurations:Release"
		links { 
			--"FreeImageLib"
		}

	filter ""