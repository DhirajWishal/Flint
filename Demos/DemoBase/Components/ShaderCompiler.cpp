// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderCompiler.hpp"

void CompileShader(const std::filesystem::path& shaderFile, const std::filesystem::path& solutionDirectory)
{
	std::string outputDirectory = "Flint\\Shaders\\" + shaderFile.parent_path().string();
	if (!std::filesystem::exists(outputDirectory))
		std::filesystem::create_directory(outputDirectory);

	std::string command = "call \""
		+ solutionDirectory.string()
		+ "\\Demos\\DemoBase\\ThirdParty\\glslangValidator\" -V \""
		+ solutionDirectory.string()
		+ "\\Demos\\DemoBase\\Shaders\\" + shaderFile.string()
		+ "\" -o \"Flint\\Shaders\\" + shaderFile.string() + ".spv\"";

	system(command.c_str());
}

void CompileAllShaders(const std::filesystem::path& solutionDirectory)
{
	std::filesystem::path searchDirectory = solutionDirectory.string() + "\\Demos\\DemoBase\\Shaders\\";

	for (auto iterator : std::filesystem::recursive_directory_iterator(searchDirectory))
	{
		if (iterator.path().extension() == ".vert" ||
			iterator.path().extension() == ".frag" ||
			iterator.path().extension() == ".comp" ||
			iterator.path().extension() == ".tesc" ||
			iterator.path().extension() == ".tese" ||
			iterator.path().extension() == ".geom")
		{
			auto parent = iterator.path().parent_path();
			auto folder = parent.string().substr(parent.string().find_last_of("\\") + 1);

			std::string outputDirectory = "Flint\\Shaders\\" + folder;
			if (!std::filesystem::exists(outputDirectory))
				std::filesystem::create_directory(outputDirectory);

			std::string command = "call \""
				+ solutionDirectory.string()
				+ "\\Demos\\DemoBase\\ThirdParty\\glslangValidator\" -V \""
				+ iterator.path().string()
				+ "\" -o \""
				+ outputDirectory
				+ "\\"
				+ iterator.path().filename().string()
				+ ".spv\"";

			system(command.c_str());
		}
	}
}