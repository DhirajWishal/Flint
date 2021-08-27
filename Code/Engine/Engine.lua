-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint Engine project description ----------

project "FlintEngine"
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
		"%{IncludeDir.Assimp}",
	}

	libdirs {
		"%{IncludeLib.Assimp}",
	}

	links { 
		"assimp-vc142-mt",
		"VulkanBackend"
	}

	filter "configurations:Debug"
	    buildoptions "/MTd"

	filter "configurations:PreRelease"
	    buildoptions "/MT"

	filter "configurations:Release"
	    buildoptions "/MT"

	filter ""
