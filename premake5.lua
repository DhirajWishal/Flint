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
	IncludeDir["Assimp"] = "$(SolutionDir)ThirdParty/Assimp/include"
	IncludeDir["shaderc"] = "$(SolutionDir)ThirdParty/shaderc/libshaderc/include"
	IncludeDir["imgui"] = "$(SolutionDir)ThirdParty/imgui/"
	IncludeDir["ImGuizmo"] = "$(SolutionDir)ThirdParty/ImGuizmo/"
	IncludeDir["glm"] = "$(SolutionDir)ThirdParty/glm/"
	IncludeDir["xxHash"] = "$(SolutionDir)ThirdParty/xxHash/"

	-- Binaries
	IncludeLib["GLFW"] = "$(SolutionDir)ThirdParty/glfw/src/Release"
	IncludeLib["Vulkan"] = "$(SolutionDir)ThirdParty/Vulkan/lib"
	IncludeLib["SPIRV_Cross"] = "$(SolutionDir)ThirdParty/SPIRV-Cross/Release"
	IncludeLib["Assimp"] = "$(SolutionDir)ThirdParty/Assimp/lib/Release"
	IncludeLib["shaderc_debug"] = "$(SolutionDir)ThirdParty/shaderc/libshaderc/Debug"
	IncludeLib["shaderc_release"] = "$(SolutionDir)ThirdParty/shaderc/libshaderc/Release"

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
		include "Code/Core/Core.lua"
		include "Code/Debugger/Debugger.lua"
		include "Code/Engine/Engine.lua"
		include "Code/GraphicsCore/GraphicsCore.lua"
		include "Code/VulkanBackend/VulkanBackend.lua"

	group "Editor"
		include "Editor/Editor.lua"

	group "Demos"
		include "Demos/SampleScene/SampleScene.lua"
		
	group ""