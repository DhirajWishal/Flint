// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Program.hpp"
#include "ShaderCode.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Rasterizing program class.
		 * This class contains the rasterizing pipeline's program which can be passed to pipelines.
		 *
		 * The vertex shader inputs need to have the proper layout locations to make sure that the bound resources are correct.
		 * The locations and attributes are specified below.
		 *  __________ __________________
		 * | Location | Vertex Attribute |
		 * |__________|__________________|
		 * |    0     |     Position     |
		 * |    1     |     Normal       |
		 * |    2     |     Tangent      |
		 * |    3     |     BiTangent    |
		 * |    4     |     Color0       |
		 * |    5     |     Color1       |
		 * |    6     |     Color2       |
		 * |    7     |     Color3       |
		 * |    8     |     Color4       |
		 * |    9     |     Color5       |
		 * |    10    |     Color6       |
		 * |    11    |     Color7       |
		 * |    12    |     Texture0     |
		 * |    13    |     Texture1     |
		 * |    14    |     Texture2     |
		 * |    15    |     Texture3     |
		 * |    16    |     Texture4     |
		 * |    17    |     Texture5     |
		 * |    18    |     Texture6     |
		 * |    19    |     Texture7     |
		 * |__________|__________________|
		 *
		 * Instance attributes follow a similar structure.
		 *  __________ ____________________
		 * | Location | Instance Attribute |
		 * |__________|____________________|
		 * |    20    |     InstanceID     |
		 * |    21    |     Position       |
		 * |    22    |     Rotation       |
		 * |    23    |     Scale          |
		 * |__________|____________________|
		 */
		class RasterizingProgram : public Program
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device to which the program is bound to.
			 * @param vertexShader The vertex shader source.
			 * @param fragmentShader The fragment shader source.
			 */
			explicit RasterizingProgram(const std::shared_ptr<Device>& pDevice, ShaderCode&& vertexShader, ShaderCode&& fragementShader)
				: Program(pDevice), m_VertexShader(std::move(vertexShader)), m_FragmentShader(std::move(fragementShader)) {}

			/**
			 * Virtual default destructor.
			 */
			virtual ~RasterizingProgram() = default;

			/**
			 * Get the vertex shader path.
			 *
			 * @return The shader path.
			 */
			[[nodsicard]] const ShaderCode& getVertexShaderPath() const { return m_VertexShader; }

			/**
			 * Get the fragment shader path.
			 *
			 * @return The shader path.
			 */
			[[nodsicard]] const ShaderCode& getFragmentShaderPath() const { return m_FragmentShader; }

			/**
			 * Get the vertex inputs.
			 *
			 * @return The vertex inputs.
			 */
			[[nodsicard]] const std::vector<VertexInput>& getVertexInputs() const { return m_VertexInputs; }

			/**
			 * Get the instance inputs.
			 *
			 * @return The instance inputs.
			 */
			[[nodsicard]] const std::vector<InstanceInput>& getInstanceInputs() const { return m_InstanceInputs; }

		protected:
			ShaderCode m_VertexShader;
			ShaderCode m_FragmentShader;

			std::vector<VertexInput> m_VertexInputs;
			std::vector<InstanceInput> m_InstanceInputs;
		};
	}
}