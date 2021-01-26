-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Shader Tools project description ----------

project "ShaderTools"
	kind "StaticLib"
	language "C++"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "On"

	targetdir "$(SolutionDir)Builds/LowLevel/Binaries/$(Configuration)-$(Platform)"
	objdir "$(SolutionDir)Builds/LowLevel/Intermediate/$(Configuration)-$(Platform)/$(ProjectName)"

	files {
		"**.txt",
		"**.cpp",
		"**.h",
		"**.lua",
		"**.txt",
		"**.md",
	}

	includedirs {
		"$(SolutionDir)Source/LowLevel/",
		"$(SolutionDir)Source/Tools/",
		"$(SolutionDir)Dependencies/ThirdParty/SPIRV-Cross/",
	}

	libdirs {
	}

	links { 
		"SPIRV-Cross"
	}