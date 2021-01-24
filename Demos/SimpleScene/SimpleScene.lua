-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Demo: SimpleScene project description ----------

project "SimpleScene"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	staticruntime "On"

	targetdir "$(SolutionDir)Builds/Demos/Binaries/$(Configuration)-$(Platform)/$(ProjectName)"
	objdir "$(SolutionDir)Builds/Demos/Intermediate/$(Configuration)-$(Platform)/$(ProjectName)"

	files {
		"**.txt",
		"**.cpp",
		"**.h",
		"src/**.cpp",
		"src/**.h",
		"**.lua"
	}

	includedirs {
		"$(SolutionDir)Source/LowLevel/",
		"$(SolutionDir)Demos/SimpleScene",
	}

	links {
		"LowLevel"
	}