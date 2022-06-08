// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/ShaderFactory/ShaderBuilder.hpp"
#include "Engine/ShaderFactory/ShaderBuildError.hpp"

#include <sstream>

namespace /* anonymous */
{
	/**
	 * Convert the DataType enum to a string.
	 *
	 * @param type The data type.
	 * @return The string.
	 */
	[[nodiscard]] std::string DataTypeToString(Flint::Core::DataType type)
	{
		switch (type)
		{
		case Flint::Core::DataType::Float:
			return "float";

		case Flint::Core::DataType::Vec2_8:
		case Flint::Core::DataType::Vec2_16:
		case Flint::Core::DataType::Vec2_32:
		case Flint::Core::DataType::Vec2_64:
			return "vec2";

		case Flint::Core::DataType::Vec3_8:
		case Flint::Core::DataType::Vec3_16:
		case Flint::Core::DataType::Vec3_32:
		case Flint::Core::DataType::Vec3_64:
			return "vec3";

		case Flint::Core::DataType::Vec4_8:
		case Flint::Core::DataType::Vec4_16:
		case Flint::Core::DataType::Vec4_32:
		case Flint::Core::DataType::Vec4_64:
		case Flint::Core::DataType::Max:
			return "vec4";

		default:
			throw Flint::ShaderBuilderError("Invalid data type!");
		}
	}
}

namespace Flint
{
	void ShaderBuilder::addInput(uint32_t location, Core::DataType type, std::string&& name)
	{
		m_Inputs.emplace_back("layout(location = " + std::to_string(location) + ") in " + DataTypeToString(type) + name);
	}

	void ShaderBuilder::addOutput(uint32_t location, Core::DataType type, std::string&& name)
	{
		m_Outputs.emplace_back("layout(location = " + std::to_string(location) + ") out " + DataTypeToString(type) + name);
	}

	std::vector<uint32_t> ShaderBuilder::compile() const
	{
		std::stringstream codeStream;

		// Combine the inputs.
		for (const auto& input : m_Inputs)
			codeStream << input << std::endl;

		// Combine the outputs.
		for (const auto& output : m_Outputs)
			codeStream << output << std::endl;

		return {};
	}
}