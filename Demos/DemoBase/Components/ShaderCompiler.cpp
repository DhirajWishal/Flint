// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderCompiler.hpp"

void CompileShader(const boost::filesystem::path& shaderFile, const boost::filesystem::path& solutionDirectory)
{
	std::string outputDirectory = "Flint\\Shaders\\" + shaderFile.parent_path().string();
	if (!boost::filesystem::exists(outputDirectory))
		boost::filesystem::create_directory(outputDirectory);

	std::string command = "call \""
		+ solutionDirectory.string()
		+ "\\Demos\\DemoBase\\ThirdParty\\glslangValidator\" -V \""
		+ solutionDirectory.string()
		+ "\\Demos\\DemoBase\\Shaders\\" + shaderFile.string()
		+ "\" -o \"Flint\\Shaders\\" + shaderFile.string() + ".spv\"";

	system(command.c_str());
}

void CompileAllShaders(const boost::filesystem::path& solutionDirectory)
{
	boost::filesystem::path searchDirectory = solutionDirectory.string() + "\\Demos\\DemoBase\\Shaders\\";

	for (auto iterator : boost::filesystem::recursive_directory_iterator(searchDirectory))
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
			if (!boost::filesystem::exists(outputDirectory))
				boost::filesystem::create_directory(outputDirectory);

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