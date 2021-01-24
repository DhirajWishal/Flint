-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Low Level project description ----------

project "LowLevel"
	kind "StaticLib"
	language "C++"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "On"

	defines {
		"GLFW_INCLUDE_VULKAN"
	}

	targetdir "$(SolutionDir)Builds/LowLevel/Binaries/$(Configuration)-$(Platform)"
	objdir "$(SolutionDir)Builds/LowLevel/Intermediate/$(Configuration)-$(Platform)/$(ProjectName)"

	files {
		"**.txt",
		"**.cpp",
		"**.h",
		"**.lua",
		"**.txt",
		"**.md",
	}

	includedirs {
		"$(SolutionDir)Source/LowLevel/",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.GLFW}",
	}

	libdirs {
	}

	links { 
		"VulkanBackend",
	}
