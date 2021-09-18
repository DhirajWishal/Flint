-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint Debugger project description ----------

project "FlintDebugger"
	kind "SharedLib"
	cppdialect "C++17"
	language "C++"
	staticruntime "On"
	systemversion "latest"

	flags { "MultiProcessorCompile" }

	targetdir "$(SolutionDir)Builds/Binaries/$(ProjectName)/$(Configuration)"
	objdir "$(SolutionDir)Builds/Intermediate/$(ProjectName)/$(Configuration)"

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
