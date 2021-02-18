// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderTools/Transpiler.h"

namespace Flint
{
	namespace ShaderTools
	{
		/**
		 * SPIR-V Transpiler object.
		 * This object transpiles SPIR-V code to GLSL and HLSL.
		 */
		class SPIRVTranspiler final : public Transpiler {
		public:
			SPIRVTranspiler() {}

			virtual std::vector<UI32> GetGLSL() override final;
			virtual std::vector<UI32> GetHLSL() override final;
		};
	}
}