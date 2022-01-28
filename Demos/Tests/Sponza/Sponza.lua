-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Sponza project description ----------

project "Sponza"
	kind "ConsoleApp"
	cppdialect "C++20"
	language "C++"
	systemversion "latest"
	staticruntime "on"

	flags { "MultiProcessorCompile" }

	targetdir "%{DebugDirectory}"
	objdir "%{wks.location}/Builds/Demos/Intermediate/%{prj.name}/%{cfg.longname}"
	debugdir "%{DebugDirectory}"

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
	    "%{IncludeLib.GLFW}",
	    "%{IncludeLib.Assimp}",
	}

	links {
		"TestBase",
	}

	filter { "files:**.cpp" }
		compileas "Module"

	filter "toolset:msc"
		postbuildcommands {
			"{Copy} \"%{wks.location}/ThirdParty/glfw/src/Release/glfw3.dll\" \"%{cfg.targetdir}\"",
			"{Copy} \"%{wks.location}/ThirdParty/Assimp/bin/Release/assimp-vc142-mt.dll\" \"%{cfg.targetdir}\"",
		}

	filter "toolset:clang or gcc"
		postbuildcommands {
			"{Copy} \"%{wks.location}/ThirdParty/glfw/src/Release/libglfw.so\" \"%{cfg.targetdir}\"",
			"{Copy} \"%{wks.location}/ThirdParty/Assimp/bin/libassimp.so\" \"%{cfg.targetdir}\"",
		}

	filter { "toolset:msc", "configurations:Debug" }
	    buildoptions "/MTd"

	filter { "toolset:msc", "configurations:PreRelease" }
	    buildoptions "/MT"

	filter { "toolset:msc", "configurations:Release" }
	    buildoptions "/MT"

	filter ""