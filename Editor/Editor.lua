-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint Editor project description ----------

project "FlintEditor"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	systemversion "latest"
	staticruntime "on"

	targetdir "$(SolutionDir)Builds/Editor/Binaries/$(ProjectName)/$(Configuration)"
	objdir "$(SolutionDir)Builds/Editor/Intermediate/$(ProjectName)/$(Configuration)"

	defines { "FLINT_SOLUTION_DIR=$(SolutionDir)" }

	postbuildcommands {
		"{COPY} \"$(SolutionDir)ThirdParty/glfw/src/Release/glfw3.dll\" \"%{cfg.targetdir}\"",
		"{COPY} \"$(SolutionDir)ThirdParty/Runtime/vulkan-1.dll\" \"%{cfg.targetdir}\"",
		"{COPY} \"$(SolutionDir)ThirdParty/Assimp/bin/Release/assimp-vc142-mt.dll\" \"%{cfg.targetdir}\"",
	}

	files {
		"**.txt",
		"**.cpp",
		"**.hpp",
		"**.h",
		"**.lua",
		"**.wfc",

		"**.vert",
		"**.frag",
		"**.comp",
		"**.tesc",
		"**.tese",
		"**.geom",
	}

	includedirs {
		"$(SolutionDir)Code",
		"$(SolutionDir)Editor",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.boost}",
	}

	libdirs {
		"%{IncludeLib.boost}",
	}

	links {
		"FlintEngine",
	}

	filter "configurations:Debug"
	    buildoptions "/MTd"

	filter "configurations:PreRelease"
	    buildoptions "/MT"

	filter "configurations:Release"
	    buildoptions "/MT"

	filter ""