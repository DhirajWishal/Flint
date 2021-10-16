-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint Graphics project description ----------

project "FlintGraphics"
	language "C++"
	kind "StaticLib"
	systemversion "latest"
	cppdialect "C++17"
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
		"%{IncludeDir.glm}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.optick}",
		"%{IncludeDir.shaderc}",
	}

	libdirs {
		"%{IncludeLib.Assimp}",
	}

	links { 
		"assimp-vc142-mt",
		"shaderc_combined",
		"VulkanBackend"
	}
	
	filter { "toolset:msc", "configurations:Debug" }
		libdirs { "%{IncludeLib.shaderc_debug}" }
	    buildoptions "/MTd"

	filter { "toolset:msc", "configurations:PreRelease" }
		libdirs { "%{IncludeLib.shaderc_release}" }
	    buildoptions "/MT"

	filter { "toolset:msc", "configurations:Release" }
		libdirs { "%{IncludeLib.shaderc_release}" }
	    buildoptions "/MT"

	filter ""
