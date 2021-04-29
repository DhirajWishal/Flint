-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint project description ----------

project "Flint"
	kind "StaticLib"
	language "C++"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "On"
	targetname "Flint"

	targetdir "$(SolutionDir)Builds/Binaries/$(ProjectName)/$(Configuration)"
	objdir "$(SolutionDir)Builds/Intermediate/$(Configuration)/$(ProjectName)"

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
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.FreeImage}",
	}

	libdirs {
		"%{IncludeLib.Assimp}",
		"%{IncludeLib.FreeImage}",
	}

	links { 
	    "VulkanBackend",
		"ShaderTools",
		"assimp-vc142-mt",
	}

	filter "configurations:Debug"
		links { 
			"FreeImageLibd"
		}

	filter "configurations:Release"
		links { 
			"FreeImageLib"
		}

	filter ""