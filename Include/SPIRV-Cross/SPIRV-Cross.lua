-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Flint Support Library: SPIRV-Cross ----------

project "SPIRV-Cross"
	kind "StaticLib"
	language "C++"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "On"
	targetname "SPIRV-Cross"

	targetdir "$(SolutionDir)Builds/ThirdParty/Binaries/$(Configuration)/$(ProjectName)"
	objdir "$(SolutionDir)Builds/ThirdParty/Intermediate/$(Configuration)/$(ProjectName)"

	files {
		"**.txt",
		"**.cpp",
		"**.h",
		"**.hpp",
		"**.lua",
		"**.txt",
		"**.md",
	}

	includedirs {
		"$(SolutionDir)Dependencies/ThirdParty/SPIRV-Cross",
	}
