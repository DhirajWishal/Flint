-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint Core project description ----------

project "FlintCore"
	language "C++"
	kind "StaticLib"
	systemversion "latest"
	cppdialect "C++20"
	staticruntime "on"

	flags { "MultiProcessorCompile" }

	targetdir "%{wks.location}/Builds/Binaries/%{cfg.longname}"
	objdir "%{wks.location}/Builds/Intermediate/%{cfg.longname}"

	files {
		"**.txt",
		"**.cpp",
		"**.hpp",
		"**.lua",
		"**.txt",
		"**.md",

	}

	includedirs {
		"%{wks.location}/Code/",
		"%{IncludeDir.xxHash}",
		"%{IncludeDir.optick}",
	}

	libdirs {
	}

	links { 
		"optick"
	}

	filter { "files:**.cpp" }
		compileas "Module"

	filter { "toolset:msc", "configurations:Debug" }
	    buildoptions "/MTd"

	filter { "toolset:msc", "configurations:PreRelease" }
	    buildoptions "/MT"

	filter { "toolset:msc", "configurations:Release" }
	    buildoptions "/MT"

	filter ""
