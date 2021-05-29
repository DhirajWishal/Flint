-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint project description ----------

project "Flint"
	language "C++"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "On"

	targetdir "$(SolutionDir)Builds/Binaries/$(Configuration)"
	objdir "$(SolutionDir)Builds/Intermediate/$(Configuration)"

	files {
		"../Include/**.h",
		"**.txt",
		"**.cpp",
		"**.h",
		"**.lua",
		"**.txt",
		"**.md",
	}

	includedirs {
		"$(SolutionDir)Source/",
		"$(SolutionDir)Include/",
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

	filter "configurations:Debug*"
		links { 
			"FreeImageLibd"
		}

	filter "configurations:Release*"
		links { 
			"FreeImageLib"
		}

	filter "configurations:*Static"
		kind "StaticLib"
		defines { "FLINT_BUILD_STATIC" }

	filter "configurations:*Shared"
		kind "SharedLib"
		defines { "FLINT_EXPORT" }

	filter ""