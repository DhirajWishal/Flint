// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Backend/Types.hpp"

#include <string>

namespace Flint
{
	/**
	 * Shader builder class.
	 * This class can be used to build shaders at runtime.
	 */
	class ShaderBuilder
	{
	public:
		/**
		 * Default constructor.
		 */
		ShaderBuilder() = default;

		/**
		 * Add a shader input to the builder.
		 *
		 * @param location The location of the input.
		 * @param type The type of the input.
		 * @param name The name of the input.
		 */
		void addInput(uint32_t location, DataType type, std::string&& name);

		/**
		 * Add a shader output to the builder.
		 *
		 * @param location The location of the output.
		 * @param type The type of the output.
		 * @param name The name of the output.
		 */
		void addOutput(uint32_t location, DataType type, std::string&& name);

		/**
		 * Compile the shader code and get the SPIR-V data.
		 *
		 * @return The compiled shader code.
		 */
		[[nodiscard]] std::vector<uint32_t> compile() const;

	private:
		std::vector<std::string> m_Inputs;
		std::vector<std::string> m_Outputs;
	};
}