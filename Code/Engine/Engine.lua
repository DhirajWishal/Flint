-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint Engine project description ----------

project "FlintEngine"
	language "C++"
	kind "StaticLib"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "on"

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
		"%{IncludeDir.glm}",
		"%{IncludeDir.Assimp}",
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

	filter "configurations:Debug"
		libdirs { "%{IncludeLib.shaderc_debug}" }
	    buildoptions "/MTd"

	filter "configurations:PreRelease"
		libdirs { "%{IncludeLib.shaderc_release}" }
	    buildoptions "/MT"

	filter "configurations:Release"
		libdirs { "%{IncludeLib.shaderc_release}" }
	    buildoptions "/MT"

	filter ""
