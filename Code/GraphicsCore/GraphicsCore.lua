-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Graphics Core project description ----------

project "GraphicsCore"
	language "C++"
	kind "StaticLib"
	systemversion "latest"
	cppdialect "C++17"
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
		"$(SolutionDir)Code/",
		"%{IncludeDir.Vulkan}",
	}

	libdirs {
	}

	links { 
		"Core",
	}

	filter "configurations:Debug"
	    buildoptions "/MTd"

	filter "configurations:PreRelease"
	    buildoptions "/MT"

	filter "configurations:Release"
	    buildoptions "/MT"

	filter ""
