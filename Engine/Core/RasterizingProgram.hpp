// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Program.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Rasterizing program class.
		 * This class contains the rasterizing pipeline's program which can be passed to pipelines.
		 */
		template<class TDevice>
		class RasterizingProgram : public Program<TDevice>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device to which the program is bound to.
			 * @param vertexShader The vertex shader source file.
			 * @param fragmentShader The fragment shader source file.
			 */
			explicit RasterizingProgram(TDevice& device, std::filesystem::path&& vertexShader, std::filesystem::path&& fragementShader)
				: Program(device), m_VertexShader(std::move(vertexShader)), m_FragmentShader(std::move(fragementShader)) {}

			/**
			 * Virtual default destructor.
			 */
			virtual ~RasterizingProgram() = default;

		protected:
			std::filesystem::path m_VertexShader;
			std::filesystem::path m_FragmentShader;
		};
	}
}