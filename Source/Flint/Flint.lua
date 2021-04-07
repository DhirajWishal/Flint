-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint project description ----------

project "Flint"
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
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.FreeImage}",
	}

	filter "configurations:Debug*"
		libdirs {
			"%{IncludeLib.Assimp}",
			"%{IncludeLib.FreeImageD}",
		}

		links { 
		    "VulkanBackend",
			"ShaderTools",
			"assimp-vc142-mt",
			"FreeImageLibd"
		}

	filter "configurations:Release*"
		libdirs {
			"%{IncludeLib.Assimp}",
			"%{IncludeLib.FreeImageR}",
		}

		links { 
		    "VulkanBackend",
			"ShaderTools",
			"assimp-vc142-mt",
			"FreeImageLib"
		}

	filter ""