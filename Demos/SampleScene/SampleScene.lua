-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Demo: Sample Scene project description ----------

project "SampleScene"
	kind "SharedLib"
	cppdialect "C++17"
	language "C++"
	staticruntime "On"
	systemversion "latest"

	flags { "MultiProcessorCompile" }

	targetdir "%{wks.location}/Builds/Demos/Binaries/%{prj.name}/%{cfg.longname}"
	objdir "%{wks.location}/Builds/Demos/Intermediate/%{prj.name}/%{cfg.longname}"

	defines { "FLINT_SOLUTION_DIR=%{wks.location}/" }

	postbuildcommands {
		"{COPY} \"%{wks.location}/ThirdParty/glfw/src/Release/glfw3.dll\" \"%{cfg.targetdir}\"",
		"{COPY} \"%{wks.location}/ThirdParty/Runtime/vulkan-1.dll\" \"%{cfg.targetdir}\"",
		"{COPY} \"%{wks.location}/ThirdParty/Assimp/bin/Release/assimp-vc142-mt.dll\" \"%{cfg.targetdir}\"",
	}

	files {
		"**.txt",
		"**.cpp",
		"**.hpp",
		"**.h",
		"**.lua",
		"**.wfc",
	}

	includedirs {
		"%{wks.location}/Code",
		"%{wks.location}/Demos/SampleScene",
		"%{IncludeDir.glm}",
	}

	libdirs {
	}

	links {
		"FlintEngine",
	}

	filter { "toolset:msc", "configurations:Debug" }
	    buildoptions "/MTd"

	filter { "toolset:msc", "configurations:PreRelease" }
	    buildoptions "/MT"

	filter { "toolset:msc", "configurations:Release" }
	    buildoptions "/MT"

	filter ""