// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

namespace Flint
{
	namespace ShaderTools
	{
		/**
		 * SPIR-V Transpiler object.
		 * This object transpiles SPIR-V code to GLSL and HLSL.
		 */
		class SPIRVTranspiler {
		public:
			SPIRVTranspiler() {}
			~SPIRVTranspiler() {}

			/**
			 * Initialize the transpiler with the shader code.
			 * Make sure that the shader code is SPIR-V.
			 * 
			 * @param pShaderCode: The pointer to the shader code.
			 */
			void Initialize(std::vector<UI32>* pShaderCode);

			/**
			 * Transpile the shader code to GLSL.
			 * 
			 * @return ShaderCode object with the GLSL shader data.
			 */
			std::vector<UI32> GetGLSL();

			/**
			 * Transpile the shader code to HLSL.
			 *
			 * @return ShaderCode object with the HLSL shader data.
			 */
			std::vector<UI32> GetHLSL();

		private:
			std::vector<UI32>* pShaderCode = nullptr;	// The shader code pointer.
		};
	}
}