-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

workspace "Flint"
	architecture "x64"

	configurations {
		"Debug",
		"PreRelease",
		"Release",
	}

	startproject "FlintEngine"

	DebugDirectory = "%{wks.location}/Workspace"

	-- Libraries
	IncludeDir = {}
	IncludeDir["GLFW"] = "%{wks.location}/ThirdParty/glfw/include"
	IncludeDir["Vulkan"] = "%{wks.location}/ThirdParty/Vulkan/include"
	IncludeDir["SPIRV_Cross"] = "%{wks.location}/ThirdParty/SPIRV-Cross/include"
	IncludeDir["Assimp"] = "%{wks.location}/ThirdParty/Assimp/include"
	IncludeDir["imgui"] = "%{wks.location}/ThirdParty/imgui/"
	IncludeDir["ImGuizmo"] = "%{wks.location}/ThirdParty/ImGuizmo/"
	IncludeDir["glm"] = "%{wks.location}/ThirdParty/glm/"
	IncludeDir["volk"] = "%{wks.location}/ThirdParty/volk/"
	IncludeDir["xxHash"] = "%{wks.location}/ThirdParty/xxHash/"
	IncludeDir["optick"] = "%{wks.location}/ThirdParty/optick/src"
	IncludeDir["vma"] = "%{wks.location}/ThirdParty/VulkanMemoryAllocator/include"
	IncludeDir["benchmark"] = "%{wks.location}/ThirdParty/benchmark/include"

	-- Binary includes directories
	IncludeLib = {}

	-- Binaries
	Binary = {}

	IncludeLib["GLFW"] = "%{wks.location}/ThirdParty/glfw/src/Release"
    IncludeLib["Vulkan"] = "%{wks.location}/ThirdParty/Vulkan/lib"
    IncludeLib["SPIRV_Cross"] = "%{wks.location}/ThirdParty/SPIRV-Cross/Release"
    IncludeLib["Assimp"] = "%{wks.location}/ThirdParty/Assimp/lib/Release"
    IncludeLib["benchmark"] = "%{wks.location}/ThirdParty/benchmark/build/src/Release"

	Binary["GLFW"] = "glfw3dll"
    Binary["Vulkan"] = "vulkan-1"
    Binary["Assimp"] = "assimp-vc142-mt"
    Binary["benchmark"] = "benchmark"

	filter "configurations:Debug"
		defines { "FLINT_DEBUG" }
		symbols "On"
		runtime "Debug"

	filter "configurations:PreRelease"
		defines { "FLINT_PRE_RELEASE" }
		optimize "On"
		runtime "Release"

	filter "configurations:Release"
		defines { "FLINT_RELEASE", "USE_OPTICK=0" }
		optimize "On"
		runtime "Release"

	filter "system:windows"
		defines { "FLINT_PLATFORM_WINDOWS" }

		--IncludeLib["GLFW"] = "%{wks.location}/ThirdParty/glfw/src/Release"
        --IncludeLib["Vulkan"] = "%{wks.location}/ThirdParty/Vulkan/lib"
        --IncludeLib["SPIRV_Cross"] = "%{wks.location}/ThirdParty/SPIRV-Cross/Release"
        --IncludeLib["Assimp"] = "%{wks.location}/ThirdParty/Assimp/lib/Release"
        --IncludeLib["benchmark"] = "%{wks.location}/ThirdParty/benchmark/build/src/Release"
		--
		--Binary["GLFW"] = "glfw3dll"
        --Binary["Vulkan"] = "vulkan-1"
        --Binary["Assimp"] = "assimp-vc142-mt"
        --Binary["benchmark"] = "benchmark"

	filter "system:linux"
		defines { "FLINT_PLATFORM_LINUX" }

		--IncludeLib["GLFW"] = "%{wks.location}/ThirdParty/glfw/src"
        --IncludeLib["Assimp"] = "%{wks.location}/ThirdParty/Assimp/lib"
        --IncludeLib["benchmark"] = "%{wks.location}/ThirdParty/benchmark/build/src"
		--
		--Binary["GLFW"] = "libglfw3"
        --Binary["Vulkan"] = "vulkan-1"
        --Binary["Assimp"] = "libassimp"
        --Binary["benchmark"] = "libbenchmark"

	filter ""

	include "Workspace/Workspace.lua"

	group "Code"
		include "Code/Audio/Audio.lua"
		include "Code/Core/Core.lua"
		include "Code/Engine/Engine.lua"
		include "Code/Engine/Engine.lua"
		include "Code/Graphics/Graphics.lua"
		include "Code/GraphicsCore/GraphicsCore.lua"
		include "Code/VulkanBackend/VulkanBackend.lua"

		group "Code/ThirdParty"
			include "Code/optick/optick.lua"

		group ""

	group "Editor"
		include "Editor/Editor.lua"

	group "Demos"
		include "Demos/SampleScene/SampleScene.lua"

		group"Demos/Tests"
			include "Demos/Tests/TestBase/TestBase.lua"
			include "Demos/Tests/ImageBasedLighting/ImageBasedLighting.lua"
			include "Demos/Tests/Sponza/Sponza.lua"
			include "Demos/Tests/SmartShaders/SmartShaders.lua"
			include "Demos/Tests/ScreenSpaceReflections/ScreenSpaceReflections.lua"
		
		group ""
	group ""
