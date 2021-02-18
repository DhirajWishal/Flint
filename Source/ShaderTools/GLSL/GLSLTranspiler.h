// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderTools/Transpiler.h"

namespace Flint
{
	namespace ShaderTools
	{
		/**
		 * Glsl transpiler object.
		 * This object is responsible of transpiling GLSL code to SPIR-V and HLSL.
		 */
		class GLSLTranspiler final : public Transpiler {
		public:
			GLSLTranspiler() {}

			virtual std::vector<UI32> GetHLSL() override final;
			virtual std::vector<UI32> GetSPIRV() override final;
		};
	}
}