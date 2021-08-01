-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Demo: SimpleScene project description ----------

project "SimpleScene"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	staticruntime "On"

	targetdir "$(SolutionDir)Builds/Demos/Binaries/$(ProjectName)/$(Configuration)"
	objdir "$(SolutionDir)Builds/Demos/Intermediate/$(ProjectName)/$(Configuration)"

	postbuildcommands {
		"{COPY} \"$(SolutionDir)Dependencies/Runtime/assimp-vc142-mt.dll\" \"%{cfg.targetdir}\"",
		"{COPY} \"$(SolutionDir)Dependencies/Runtime/glfw3.dll\" \"%{cfg.targetdir}\"",
		"{COPY} \"$(SolutionDir)Dependencies/Runtime/shaderc_shared.dll\" \"%{cfg.targetdir}\"",
		"{COPY} \"$(SolutionDir)Dependencies/Runtime/vulkan-1.dll\" \"%{cfg.targetdir}\"",
		"{COPY} \"$(SolutionDir)Dependencies/Runtime/xxhash.dll\" \"%{cfg.targetdir}\"",
	}

	files {
		"**.txt",
		"**.cpp",
		"**.hpp",
		"**.h",
		"**.lua",
		"**.wfc",
	}

	includedirs {
		"$(SolutionDir)Code",
		"$(SolutionDir)Demos/ThirdParty/glm",
		"$(SolutionDir)Demos/SimpleScene",
		"%{IncludeDir.Assimp}",
	}

	libdirs {
		"%{IncludeLib.Assimp}",
	}

	links {
		"assimp-vc142-mt",
		"Flint",
	}

	filter "configurations:Debug"
	    buildoptions "/MTd"

	filter "configurations:PreRelease"
	    buildoptions "/MT"

	filter "configurations:Release"
	    buildoptions "/MT"

	filter ""