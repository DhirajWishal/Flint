-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Test Base project description ----------

project "TestBase"
	kind "StaticLib"
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

		"$(SolutionDir)ThirdParty/imgui/imgui.cpp",
		"$(SolutionDir)ThirdParty/imgui/imgui_demo.cpp",
		"$(SolutionDir)ThirdParty/imgui/imgui_draw.cpp",
		"$(SolutionDir)ThirdParty/imgui/imgui_tables.cpp",
		"$(SolutionDir)ThirdParty/imgui/imgui_widgets.cpp",

		"$(SolutionDir)ThirdParty/ImGuizmo/GraphEditor.cpp",
		"$(SolutionDir)ThirdParty/ImGuizmo/ImCurveEdit.cpp",
		"$(SolutionDir)ThirdParty/ImGuizmo/ImGradient.cpp",
		"$(SolutionDir)ThirdParty/ImGuizmo/ImGuizmo.cpp",
		"$(SolutionDir)ThirdParty/ImGuizmo/ImSequencer.cpp",
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
		"FlintEngine",
	}

	filter "configurations:Debug"
	    buildoptions "/MTd"

	filter "configurations:PreRelease"
	    buildoptions "/MT"

	filter "configurations:Release"
	    buildoptions "/MT"

	filter ""