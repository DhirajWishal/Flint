// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"

namespace Flint
{
	/**
	 * Vertex shader class.
	 */
	class VertexShader final : public Shader
	{
		/**
		 * Vertex input attribute structure.
		 */
		struct VertexInputAttribute final
		{
			uint32_t m_Location = 0;
			VertexAttribute m_Attribute = VertexAttribute::Position;
			DataType m_DataType = DataType::Vec3_32;
		};

		/**
		 * Instance input attribute structure.
		 */
		struct InstanceInputAttribute final
		{
			uint32_t m_Location = 0;
			InstanceAttribute m_Attribute = InstanceAttribute::Position;
			DataType m_DataType = DataType::Vec3_32;
		};

	public:
		// We are using the same constructor(s) as the super class.
		using Shader::Shader;

		/**
		 * Default destructor.
		 */
		~VertexShader() = default;

		/**
		 * Add a vertex input to the shader.
		 *
		 * @param location The location of the attribute.
		 * @param attribute The attribute in that location.
		 * @param type The type of the attribute.
		 */
		void addVertexInput(uint32_t location, VertexAttribute attribute, DataType type) { m_VertexInputs.emplace_back(location, attribute, type); }

		/**
		 * Check if the shader has vertex inputs.
		 *
		 * @return Boolean value.
		 */
		[[nodiscard]] bool hasVertexInputs() const noexcept { return !m_VertexInputs.empty(); }

		/**
		 * Get the vertex inputs from the shader.
		 *
		 * @return The vertex input attributes.
		 */
		[[nodiscard]] std::vector<VertexInputAttribute>& getVertexInputs() noexcept { return m_VertexInputs; }

		/**
		 * Get the vertex inputs from the shader.
		 *
		 * @return The vertex input attributes.
		 */
		[[nodiscard]] const std::vector<VertexInputAttribute>& getVertexInputs() const noexcept { return m_VertexInputs; }

		/**
		 * Add a instance input to the shader.
		 *
		 * @param location The location of the attribute.
		 * @param attribute The attribute in that location.
		 * @param type The type of the attribute.
		 */
		void addInstanceInput(uint32_t location, InstanceAttribute attribute, DataType type) { m_InstanceInputs.emplace_back(location, attribute, type); }

		/**
		 * Check if the shader has vertex inputs.
		 *
		 * @return Boolean value.
		 */
		[[nodiscard]] bool hasInstanceInputs() const noexcept { return !m_InstanceInputs.empty(); }

		/**
		 * Get the instance inputs from the shader.
		 *
		 * @return The instance input attributes.
		 */
		[[nodiscard]] std::vector<InstanceInputAttribute>& getInstanceInputs() noexcept { return m_InstanceInputs; }

		/**
		 * Get the instance inputs from the shader.
		 *
		 * @return The instance input attributes.
		 */
		[[nodiscard]] const std::vector<InstanceInputAttribute>& getInstanceInputs() const noexcept { return m_InstanceInputs; }

	private:
		std::vector<VertexInputAttribute> m_VertexInputs;
		std::vector<InstanceInputAttribute> m_InstanceInputs;
	};
}