-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

workspace "Flint"
	architecture "x64"

	configurations {
		"Debug",
		"PreRelease",
		"Release",
	}

	platforms {
		"Windows",
		"Linux"
	}

	startproject "Flint"

	IncludeDir = {}
	IncludeLib = {}

	-- Libraries
	IncludeDir["GLFW"] = "$(SolutionDir)ThirdParty/glfw/include"
	IncludeDir["Vulkan"] = "$(SolutionDir)ThirdParty/Vulkan/include"
	IncludeDir["SPIRV_Cross"] = "$(SolutionDir)ThirdParty/SPIRV-Cross/include"

	IncludeDir["imgui"] = "$(SolutionDir)Demos/ThirdParty/imgui/"
	IncludeDir["ImGuizmo"] = "$(SolutionDir)Demos/ThirdParty/ImGuizmo/"
	IncludeDir["glm"] = "$(SolutionDir)Demos/ThirdParty/glm/"
	IncludeDir["Assimp"] = "$(SolutionDir)Demos/ThirdParty/Assimp/include"

	-- Binaries
	IncludeLib["GLFW"] = "$(SolutionDir)ThirdParty/glfw/src/Release"
	IncludeLib["Vulkan"] = "$(SolutionDir)ThirdParty/Vulkan/lib"
	IncludeLib["SPIRV_Cross"] = "$(SolutionDir)ThirdParty/SPIRV-Cross/Release"

	IncludeLib["Assimp"] = "$(SolutionDir)Demos/ThirdParty/Assimp/lib/Release"

	filter "configurations:Debug"
		defines { "FLINT_DEBUG" }
		symbols "On"
		runtime "Debug"

	filter "configurations:PreRelease"
		defines { "FLINT_PRE_RELEASE" }
		optimize "On"
		runtime "Release"

	filter "configurations:Release"
		defines { "FLINT_RELEASE" }
		optimize "On"
		runtime "Release"

	filter "platforms:Windows"
		system "windows"
		defines { "FLINT_PLATFORM_WINDOWS" }

	filter "platforms:Linux"
		system "linux"
		defines { "FLINT_PLATFORM_LINUX" }

	filter ""

	group "Code"
		include "Code/Flint.lua"

	group "Editor"
		include "Editor/Editor.lua"

	group "Demos"
		include "Demos/ComputeShader/ComputeShader.lua"
		include "Demos/DemoBase/DemoBase.lua"
		include "Demos/SimpleScene/SimpleScene.lua"
		include "Demos/Skybox/Skybox.lua"
		include "Demos/ShadowMap/ShadowMap.lua"
		include "Demos/PointShadowMap/PointShadowMap.lua"
		include "Demos/MousePicking/MousePicking.lua"
		
	group ""