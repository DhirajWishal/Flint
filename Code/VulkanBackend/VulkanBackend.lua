-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Vulkan Backend project description ----------

project "VulkanBackend"
	language "C++"
	kind "StaticLib"
	systemversion "latest"
	cppdialect "C++20"
	staticruntime "on"

	targetdir "$(SolutionDir)Builds/Binaries/$(Configuration)"
	objdir "$(SolutionDir)Builds/Intermediate/$(Configuration)"

	files {
		"**.txt",
		"**.cpp",
		"**.hpp",
		"**.lua",
		"**.txt",
		"**.md",

		"$(SolutionDir)ThirdParty/SPIRV-Reflect/spirv_reflect.c",
		"$(SolutionDir)ThirdParty/SPIRV-Reflect/spirv_reflect.h",
	}

	includedirs {
		"$(SolutionDir)Code/",
		"$(SolutionDir)ThirdParty/SPIRV-Reflect",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.GLFW}",
	}

	libdirs {
		"%{IncludeLib.GLFW}",
		"%{IncludeLib.Vulkan}",
	}

	links { 
		"GraphicsCore",

		"glfw3dll",
		"vulkan-1",
	}

	filter "configurations:Debug"
	    buildoptions "/MTd"

	filter "configurations:PreRelease"
	    buildoptions "/MT"

	filter "configurations:Release"
	    buildoptions "/MT"

	filter ""