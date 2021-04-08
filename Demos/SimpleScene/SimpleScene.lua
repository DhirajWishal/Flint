-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Demo: SimpleScene project description ----------

project "SimpleScene"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	staticruntime "On"

	targetdir "$(SolutionDir)Builds/Demos/Binaries/$(ProjectName)/$(Configuration)-$(Platform)"
	objdir "$(SolutionDir)Builds/Demos/Intermediate/$(ProjectName)/$(Configuration)-$(Platform)"

	--postbuildcommands {
	--	"{COPY} $(SolutionDir)Dependencies/Runtime/assimp-vc142-mt.dll %{cfg.targetdir}",
	--	"{COPY} $(SolutionDir)Dependencies/Runtime/glfw3.dll %{cfg.targetdir}",
	--	"{COPY} $(SolutionDir)Dependencies/Runtime/shaderc_shared.dll %{cfg.targetdir}",
	--	"{COPY} $(SolutionDir)Dependencies/Runtime/vulkan-1.dll %{cfg.targetdir}",
	--	"{COPY} $(SolutionDir)Dependencies/Runtime/xxhash.dll %{cfg.targetdir}",
	--}

	files {
		"**.txt",
		"**.cpp",
		"**.h",
		"src/**.cpp",
		"src/**.h",
		"**.lua",
		"**.wfc",
	}

	includedirs {
		"$(SolutionDir)Source/",
		"$(SolutionDir)Demos/SimpleScene",
	}

	links {
		"Flint",
	}

	filter "configurations:Debug*"
	    buildoptions "/MTd"

	filter "configurations:Release*"
	    buildoptions "/MT"

	filter ""