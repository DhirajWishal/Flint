-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- optick project description ----------

project "optick"
	language "C++"
	kind "StaticLib"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "on"

	flags { "MultiProcessorCompile" }

	targetdir "$(SolutionDir)Builds/Binaries/$(Configuration)"
	objdir "$(SolutionDir)Builds/Intermediate/$(Configuration)"

	files {
		"**.txt",
		"**.cpp",
		"**.hpp",
		"**.lua",
		"**.txt",
		"**.md",

		"../../ThirdParty/optick/src/*.cpp",
		"../../ThirdParty/optick/src/*.h",
	}

	includedirs {
		"%{IncludeDir.optick}",
		"%{IncludeDir.Vulkan}",
	}

	libdirs {
	}

	links { 
	}

	filter "configurations:Debug"
	    buildoptions "/MTd"

	filter "configurations:PreRelease"
	    buildoptions "/MT"

	filter "configurations:Release"
	    buildoptions "/MT"

	filter ""
