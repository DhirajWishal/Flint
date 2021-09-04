-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Demo: Compute Shader project description ----------

project "ComputeShader"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	staticruntime "On"
	systemversion "latest"

	targetdir "$(SolutionDir)Builds/Demos/Binaries/$(ProjectName)/$(Configuration)"
	objdir "$(SolutionDir)Builds/Demos/Intermediate/$(ProjectName)/$(Configuration)"

	defines { "FLINT_SOLUTION_DIR=$(SolutionDir)" }

	postbuildcommands {
		"{Copy} \"$(SolutionDir)ThirdParty/glfw/src/Release/glfw3.dll\" \"%{cfg.targetdir}\"",
		"{Copy} \"$(SolutionDir)ThirdParty/Runtime/vulkan-1.dll\" \"%{cfg.targetdir}\"",
		"{Copy} \"$(SolutionDir)Demos/ThirdParty/Assimp/bin/Release/assimp-vc142-mt.dll\" \"%{cfg.targetdir}\"",
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
		"$(SolutionDir)Code",
		"$(SolutionDir)Demos/ComputeShader",
		"$(SolutionDir)Demos/DemoBase",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Assimp}",
	}

	libdirs {
	}

	links {
		"DemoBase",
	}

	filter "configurations:Debug"
	    buildoptions "/MTd"

	filter "configurations:PreRelease"
	    buildoptions "/MT"

	filter "configurations:Release"
	    buildoptions "/MT"

	filter ""