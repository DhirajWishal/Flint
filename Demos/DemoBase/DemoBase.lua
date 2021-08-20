-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Demo Base project description ----------

project "DemoBase"
	language "C++"
	kind "StaticLib"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "On"

	targetdir "$(SolutionDir)Builds/Demos/Binaries/$(ProjectName)/$(Configuration)"
	objdir "$(SolutionDir)Builds/Demos/Intermediate/$(ProjectName)/$(Configuration)"

	files {
		"**.txt",
		"**.cpp",
		"**.hpp",
		"**.h",
		"**.lua",
		"**.wfc",

		"**.exe",

		"**.vert",
		"**.frag",
		"**.comp",
		"**.tesc",
		"**.tese",
		"**.geom",

		"$(SolutionDir)Demos/ThirdParty/imgui/imgui.cpp",
		"$(SolutionDir)Demos/ThirdParty/imgui/imgui_demo.cpp",
		"$(SolutionDir)Demos/ThirdParty/imgui/imgui_draw.cpp",
		"$(SolutionDir)Demos/ThirdParty/imgui/imgui_tables.cpp",
		"$(SolutionDir)Demos/ThirdParty/imgui/imgui_widgets.cpp",

		"$(SolutionDir)Demos/ThirdParty/ImGuizmo/GraphEditor.cpp",
		"$(SolutionDir)Demos/ThirdParty/ImGuizmo/ImCurveEdit.cpp",
		"$(SolutionDir)Demos/ThirdParty/ImGuizmo/ImGradient.cpp",
		"$(SolutionDir)Demos/ThirdParty/ImGuizmo/ImGuizmo.cpp",
		"$(SolutionDir)Demos/ThirdParty/ImGuizmo/ImSequencer.cpp",
	}

	includedirs {
		"$(SolutionDir)Code",
		"$(SolutionDir)Demos/DemoBase",
		"%{IncludeDir.ImGuizmo}",
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

	filter ""