// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/ShaderDigest.h"

namespace Flint
{
	namespace ShaderTools
	{
		/**
		 * SPIR-V Reflection object.
		 * This object is used to perform reflection on SPIR-V shader code.
		 */
		class SPIRVReflection {
		public:
			SPIRVReflection() {}
			~SPIRVReflection() {}

			/**
			 * Perform reflection on the shader code.
			 * Make sure that the shader code type is SPIR-V.
			 *
			 * @param mShaderCode: The shader code to perform reflection.
			 * @param location: The location of the shader code.
			 * @return ShaderDigest object.
			 */
			ShaderDigest Reflect(const std::vector<UI32>& mShaderCode, ShaderLocation location);
		};
	}
}