-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint Audio project description ----------

project "FlintAudio"
	language "C++"
	kind "StaticLib"
	systemversion "latest"
	cppdialect "C++20"
	staticruntime "on"

	flags { "MultiProcessorCompile" }

	targetdir "$(SolutionDir)Builds/Binaries/%{cfg.longname}"
	objdir "$(SolutionDir)Builds/Intermediate/%{cfg.longname}"

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
	}

	libdirs {
	}

	links { 
		"FlintCore",
	}

	filter { "toolset:msc", "configurations:Debug" }
	    buildoptions "/MTd"

	filter { "toolset:msc", "configurations:PreRelease" }
	    buildoptions "/MT"

	filter { "toolset:msc", "configurations:Release" }
	    buildoptions "/MT"

	filter ""
