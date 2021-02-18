// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

namespace Flint
{
	namespace ShaderTools
	{
		/**
		 * Transpiler base object.
		 * Transpilers convert shader code from one format to another.
		 */
		class Transpiler {
		public:
			Transpiler() {}

			/**
			 * Initialize the transpiler with the shader code.
			 *
			 * @param pShaderCode: The pointer to the shader code.
			 */
			void Initialize(std::vector<UI32>* pShaderCode);

			/**
			 * Transpile the shader code to GLSL.
			 *
			 * @return GLSL shader data.
			 */
			virtual std::vector<UI32> GetGLSL() { return std::vector<UI32>(); }

			/**
			 * Transpile the shader code to HLSL.
			 *
			 * @return HLSL shader data.
			 */
			virtual std::vector<UI32> GetHLSL() { return std::vector<UI32>(); }

			/**
			 * Transpile the shader code to SPIRV.
			 *
			 * @return SPIRV shader data.
			 */
			virtual std::vector<UI32> GetSPIRV() { return std::vector<UI32>(); }

		protected:
			std::vector<UI32>* pShaderCode = nullptr;
		};
	}
}