-- Copyright 2021 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Vulkan Backend project description ----------

project "VulkanBackend"
	language "C++"
	kind "StaticLib"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "on"

	flags { "MultiProcessorCompile" }

	targetdir "%{wks.location}/Builds/Binaries/%{cfg.longname}"
	objdir "%{wks.location}/Builds/Intermediate/%{cfg.longname}"

	files {
		"**.txt",
		"**.cpp",
		"**.hpp",
		"**.lua",
		"**.txt",
		"**.md",

		"%{wks.location}/ThirdParty/SPIRV-Reflect/spirv_reflect.c",
		"%{wks.location}/ThirdParty/SPIRV-Reflect/spirv_reflect.h",

		"%{wks.location}/ThirdParty/volk/volk.c",
		"%{wks.location}/ThirdParty/volk/volk.h",
	}

	includedirs {
		"%{wks.location}/Code/",
		"%{wks.location}/ThirdParty/SPIRV-Reflect",
		"%{IncludeDir.volk}",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.optick}",
		"%{IncludeDir.vma}",
	}

	libdirs {
		"%{IncludeLib.GLFW}",
		"%{IncludeLib.Vulkan}",
	}

	links { 
		"GraphicsCore",
		"%{Binary.GLFW}",
	}

	filter { "toolset:msc", "configurations:Debug" }
	    buildoptions "/MTd"

	filter { "toolset:msc", "configurations:PreRelease" }
	    buildoptions "/MT"

	filter { "toolset:msc", "configurations:Release" }
	    buildoptions "/MT"

	filter ""