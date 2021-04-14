-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Core project description ----------

project "Core"
	kind "StaticLib"
	language "C++"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "On"
	targetname "Core"

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
	}

	libdirs {
	}

	links { 
	}