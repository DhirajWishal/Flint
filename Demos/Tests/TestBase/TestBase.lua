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

	targetdir "%{wks.location}/Builds/Demos/Binaries/%{prj.name}/%{cfg.longname}"
	objdir "%{wks.location}/Builds/Demos/Intermediate/%{prj.name}/%{cfg.longname}"

	defines { "FLINT_SOLUTION_DIR=%{wks.location}/" }

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
		"%{wks.location}/Demos/Tests",
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

	filter "toolset:msc"
		postbuildcommands {
			"{Copy} \"%{wks.location}/ThirdParty/glfw/src/Release/glfw3.dll\" \"%{cfg.targetdir}\"",
			"{Copy} \"%{wks.location}/ThirdParty/Runtime/vulkan-1.dll\" \"%{cfg.targetdir}\"",
			"{Copy} \"%{wks.location}/ThirdParty/Assimp/bin/Release/assimp-vc142-mt.dll\" \"%{cfg.targetdir}\"",
		}

	filter "toolset:clang or gcc"
		postbuildcommands {
			"{Copy} \"%{wks.location}/ThirdParty/glfw/src/Release/glibglfw.so\" \"%{cfg.targetdir}\"",
			--"{Copy} \"%{wks.location}/ThirdParty/Runtime/vulkan-1.dll\" \"%{cfg.targetdir}\"",
			"{Copy} \"%{wks.location}/ThirdParty/Assimp/bin/libassimp.so\" \"%{cfg.targetdir}\"",
		}

	filter { "toolset:msc", "configurations:Debug" }
	    buildoptions "/MTd"

	filter { "toolset:msc", "configurations:PreRelease" }
	    buildoptions "/MT"

	filter { "toolset:msc", "configurations:Release" }
	    buildoptions "/MT"

	filter ""