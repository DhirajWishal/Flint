-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

workspace "Flint"
	architecture "x64"

	configurations {
		"Debug",
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
	IncludeDir["GLFW"] = "$(SolutionDir)Dependencies/ThirdParty/glfw/include"
	IncludeDir["GLEW"] = "$(SolutionDir)Dependencies/GLEW/include"
	IncludeDir["stb"] = "$(SolutionDir)Dependencies/ThirdParty/stb"
	IncludeDir["glm"] = "$(SolutionDir)Dependencies/ThirdParty/gli/external/glm"
	IncludeDir["gli"] = "$(SolutionDir)Dependencies/ThirdParty/gli"
	IncludeDir["zlib"] = "$(SolutionDir)Dependencies/Libraries/External/zlib/"
	IncludeDir["Vulkan"] = "$(SolutionDir)Dependencies/ThirdParty/Vulkan/include"
	IncludeDir["SPIRVTools"] = "$(SolutionDir)Dependencies/ThirdParty/SPIRV-Tools/include"
	IncludeDir["glslang"] = "$(SolutionDir)Dependencies/ThirdParty/glslang/"
	IncludeDir["FreeImage"] = "$(SolutionDir)Dependencies/ThirdParty/FreeImage/Include"
	IncludeDir["SDL2"] = "$(SolutionDir)Dependencies/ThirdParty/SDL2-2.0.12/include"
	IncludeDir["xxhash"] = "$(SolutionDir)Dependencies/ThirdParty/xxhash/include"
	IncludeDir["glslang"] = "$(SolutionDir)Dependencies/ThirdParty/glslang/include"
	IncludeDir["shaderc"] = "$(SolutionDir)Dependencies/ThirdParty/shaderc/Include"
	
	IncludeDir["jpeg"] = "$(SolutionDir)Dependencies/ThirdParty/gil/jpeg-6b"
	IncludeDir["irrKlang"] = "$(SolutionDir)Dependencies/Libraries/Local/irrKlang/include"
	IncludeDir["Assimp"] = "$(SolutionDir)Dependencies/ThirdParty/Assimp/include"
	
	-- Binaries
	IncludeLib["GLFW"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/GLFW/lib-vc2019"
	IncludeLib["GLEW"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/GLEW/Release/x64"
	IncludeLib["Vulkan"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/Vulkan"
	IncludeLib["DirectX"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/DirectX12"
	IncludeLib["irrKlang"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/irrKlang"
	IncludeLib["Assimp"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/Assimp"
	IncludeLib["SPIRVTools"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/SPIRV-Tools/"
	IncludeLib["SDL2"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/SDL2-2.0.12/bin/x64/"
	IncludeLib["xxhash"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/xxhash/lib/"
	IncludeLib["glslang"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/glslang/"
	IncludeLib["shaderc"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/shaderc/"
	
	IncludeLib["zlib"] = ""	-- TODO

	filter "configurations:Debug"
		defines { "FLINT_DEBUG" }
		symbols "On"
		IncludeLib["FreeImage"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/FreeImage/Debug"

	filter "configurations:Release"
		defines { "FLINT_RELEASE" }
		optimize "On"
		IncludeLib["FreeImage"] = "$(SolutionDir)Dependencies/ThirdParty/Binaries/FreeImage/Release"

	filter "platforms:Windows"
		system "windows"
		defines { "FLINT_PLATFORM_WINDOWS" }

	filter "platforms:Linux"
		system "linux"
		defines { "FLINT_PLATFORM_LINUX" }

	filter ""

	group "Demos"
		include "Demos/SimpleScene/SimpleScene.lua"
	
	group "Engine"
		include "Source/Flint/Flint.lua"
	
	group "LowLevel"
		include "Source/Core/Core.lua"
		include "Source/VulkanBackend/VulkanBackend.lua"
	
	group "ThirdParty"
		include "Dependencies/ThirdParty/SPIRV-Cross/SPIRV-Cross.lua"
	
	group "Tools"
		include "Source/ShaderTools/ShaderTools.lua"
	
	group ""