-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint Editor project description ----------

project "FlintEditor"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	systemversion "latest"
	staticruntime "on"

	flags { "MultiProcessorCompile" }

	targetdir "%{wks.location}/Builds/Editor/Binaries/%{prj.name}/%{cfg.longname}"
	objdir "%{wks.location}/Builds/Editor/Intermediate/%{prj.name}/%{cfg.longname}"

	defines { "FLINT_SOLUTION_DIR=%{wks.location}/" }

	postbuildcommands {
		"{Copy} \"%{wks.location}/ThirdParty/glfw/src/Release/glfw3.dll\" \"%{cfg.targetdir}\"",
		"{Copy} \"%{wks.location}/ThirdParty/Runtime/vulkan-1.dll\" \"%{cfg.targetdir}\"",
		"{Copy} \"%{wks.location}/ThirdParty/Assimp/bin/Release/assimp-vc142-mt.dll\" \"%{cfg.targetdir}\"",
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

		"%{wks.location}/ThirdParty/imgui/imgui.cpp",
		"%{wks.location}/ThirdParty/imgui/imgui_demo.cpp",
		"%{wks.location}/ThirdParty/imgui/imgui_draw.cpp",
		"%{wks.location}/ThirdParty/imgui/imgui_tables.cpp",
		"%{wks.location}/ThirdParty/imgui/imgui_widgets.cpp",

		"%{wks.location}/ThirdParty/ImGuizmo/GraphEditor.cpp",
		"%{wks.location}/ThirdParty/ImGuizmo/ImCurveEdit.cpp",
		"%{wks.location}/ThirdParty/ImGuizmo/ImGradient.cpp",
		"%{wks.location}/ThirdParty/ImGuizmo/ImGuizmo.cpp",
		"%{wks.location}/ThirdParty/ImGuizmo/ImSequencer.cpp",
	}

	includedirs {
		"%{wks.location}/Code",
		"%{wks.location}/Editor",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
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