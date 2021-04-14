-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Vulkan Backend project description ----------

project "VulkanBackend"
	kind "StaticLib"
	language "C++"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "On"
	targetname "VulkanBackend"

	defines {
		"GLFW_INCLUDE_VULKAN"
	}

	targetdir "$(SolutionDir)Builds/Binaries/$(ProjectName)/$(Configuration)"
	objdir "$(SolutionDir)Builds/Intermediate/$(Configuration)/$(ProjectName)"

	files {
		"**.txt",
		"**.cpp",
		"**.h",
		"**.c",
		"**.lua",
		"**.txt",
		"**.md",
	}

	includedirs {
		"$(SolutionDir)Source/",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.GLFW}",
	}

	libdirs {
		"%{IncludeLib.GLFW}",
		"%{IncludeLib.Vulkan}",
	}

	links { 
		"Core",
		"glfw3dll",
		"vulkan-1",
	}
