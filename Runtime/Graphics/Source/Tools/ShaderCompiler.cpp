// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Tools/ShaderCompiler.hpp"
#include "GraphicsCore/Device.hpp"

#include <fstream>
#include <iostream>
#include <random>

namespace Flint
{
	namespace Helpers
	{
		const char* GetShaderStageString(const ShaderType shaderType)
		{
			switch (shaderType)
			{
			case ShaderType::Vertex:
				return "vert";
				break;

			case ShaderType::TessellationControl:
				return "tesc";

			case ShaderType::TessellationEvaluation:
				return "tese";

			case ShaderType::Geometry:
				return "geom";

			case ShaderType::Fragment:
				return "frag";

			case ShaderType::Compute:
				return "comp";

			case ShaderType::RayGen:
				return "rgen";

			case ShaderType::AnyHit:
				return "rahit";

			case ShaderType::ClosestHit:
				return "rchit";

			case ShaderType::RayMiss:
				return "rmiss";

			default:
				throw std::invalid_argument("Shader type is invalid or undefined!");
			}
		}

		void DumpToFile(const std::string& code, const std::string& fileName)
		{
			std::fstream file(fileName, std::ios::out | std::ios::binary);

			if (file.is_open())
				throw std::runtime_error("Failed to open the required file!");

			file.write(code.c_str(), code.size());
			file.close();
		}
	}

	ShaderCompiler::ShaderCompiler(const std::filesystem::path& shaderFile, const ShaderCodeType codeType, const ShaderType shaderType, std::filesystem::path compilerTool)
		: mCodeType(codeType), mType(shaderType)
	{
		static std::default_random_engine RandomEngine = {};

		const std::string stage = Helpers::GetShaderStageString(shaderType);
		const std::string outputFileName = std::to_string(RandomEngine());
		const std::string command = "call " + compilerTool.make_preferred().string() + " -V -S " + stage + " " + shaderFile.string() + " -o " + outputFileName;

		// Issue the command.
		if (std::system(command.c_str()) != 0)
			throw std::runtime_error("Could not compile the shader file!");

		// Load the shade file.
		std::ifstream file(outputFileName, std::ios::in | std::ios::binary | std::ios::ate);

		if (!file.is_open())
			throw std::invalid_argument("Provided asset file is not available!");

		const uint64 size = file.tellg();
		file.seekg(0);

		// Get the shader code.
		mShaderCode.resize(size);
		file.read(reinterpret_cast<char*>(mShaderCode.data()), size);
		file.close();

		// Remove the temporary file.
		std::filesystem::remove(outputFileName);
	}

	ShaderCompiler::ShaderCompiler(const std::string& shaderCode, const ShaderCodeType codeType, const ShaderType shaderType, std::filesystem::path compilerTool)
		: mCodeType(codeType), mType(shaderType)
	{
		static std::default_random_engine RandomEngine = {};

		const std::string outputFileName = std::to_string(RandomEngine());
		const std::string stage = Helpers::GetShaderStageString(shaderType);
		const std::string inputFile = outputFileName + ".in";
		const std::string outputFile = outputFileName + ".out";
		const std::string command = "call " + compilerTool.make_preferred().string() + " -V -S " + stage + " " + inputFile + " -o " + outputFileName;

		// Dump the code data to a file.
		Helpers::DumpToFile(shaderCode, inputFile);

		// Execute the command.
		if (std::system(command.c_str()) != 0)
			throw std::runtime_error("Could not compile the shader file!");

		// Load the shade file.
		std::ifstream file(outputFileName, std::ios::in | std::ios::binary | std::ios::ate);

		if (!file.is_open())
			throw std::invalid_argument("Provided asset file is not available!");

		const uint64 size = file.tellg();
		file.seekg(0);

		// Get the shader code.
		mShaderCode.resize(size);
		file.read(reinterpret_cast<char*>(mShaderCode.data()), size);
		file.close();

		// Remove the temporary file.
		std::filesystem::remove(outputFile);
	}

	std::shared_ptr<Shader> ShaderCompiler::CreateShader(const std::shared_ptr<Device>& pDevice) const
	{
		return pDevice->CreateShader(mType, GetShaderCode());
	}
}