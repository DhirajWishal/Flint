-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Sponza project description ----------

project "Sponza"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	systemversion "latest"
	staticruntime "on"

	flags { "MultiProcessorCompile" }

	targetdir "$(SolutionDir)Builds/Demos/Binaries/$(ProjectName)/$(Configuration)"
	objdir "$(SolutionDir)Builds/Demos/Intermediate/$(ProjectName)/$(Configuration)"

	defines { "FLINT_SOLUTION_DIR=$(SolutionDir)" }

	postbuildcommands {
		"{Copy} \"$(SolutionDir)ThirdParty/glfw/src/Release/glfw3.dll\" \"%{cfg.targetdir}\"",
		"{Copy} \"$(SolutionDir)ThirdParty/Runtime/vulkan-1.dll\" \"%{cfg.targetdir}\"",
		"{Copy} \"$(SolutionDir)ThirdParty/Assimp/bin/Release/assimp-vc142-mt.dll\" \"%{cfg.targetdir}\"",
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
		"$(SolutionDir)Demos/Tests",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.optick}",
	}

	libdirs {
	}

	links {
		"TestBase",
	}

	filter "configurations:Debug"
	    buildoptions "/MTd"

	filter "configurations:PreRelease"
	    buildoptions "/MT"

	filter "configurations:Release"
	    buildoptions "/MT"

	filter ""