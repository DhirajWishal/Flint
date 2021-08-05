-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Demo: SimpleScene project description ----------

project "SimpleScene"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	staticruntime "On"
	systemversion "latest"

	targetdir "$(SolutionDir)Builds/Demos/Binaries/$(ProjectName)/$(Configuration)"
	objdir "$(SolutionDir)Builds/Demos/Intermediate/$(ProjectName)/$(Configuration)"

	postbuildcommands {
		"{COPY} \"$(SolutionDir)ThirdParty/glfw/src/Release/glfw3.dll\" \"%{cfg.targetdir}\"",
		"{COPY} \"$(SolutionDir)ThirdParty/Runtime/vulkan-1.dll\" \"%{cfg.targetdir}\"",
		"{COPY} \"$(SolutionDir)Demos/ThirdParty/Assimp/bin/Release/assimp-vc142-mt.dll\" \"%{cfg.targetdir}\"",
	}

	files {
		"**.txt",
		"**.cpp",
		"**.hpp",
		"**.h",
		"**.lua",
		"**.wfc",

		"$(SolutionDir)Demos/ThirdParty/imgui/imgui.cpp",
		"$(SolutionDir)Demos/ThirdParty/imgui/imgui_demo.cpp",
		"$(SolutionDir)Demos/ThirdParty/imgui/imgui_draw.cpp",
		"$(SolutionDir)Demos/ThirdParty/imgui/imgui_tables.cpp",
		"$(SolutionDir)Demos/ThirdParty/imgui/imgui_widgets.cpp",
	}

	includedirs {
		"$(SolutionDir)Code",
		"$(SolutionDir)Demos/SimpleScene",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
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