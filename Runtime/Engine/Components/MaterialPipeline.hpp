// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "ComponentBase.hpp"

namespace Flint
{
	namespace Components
	{
		/**
		 * Material pipeline structure.
		 */
		struct MaterialPipeline
		{
			std::unique_ptr<GraphicsPipeline> pPipeline = nullptr;
			std::unique_ptr<Shader> pVertexShader = nullptr;
			std::unique_ptr<Shader> pFragmentShader = nullptr;
			std::unique_ptr<Shader> pTessellationControlShader = nullptr;
			std::unique_ptr<Shader> pTessellationEvaluationShader = nullptr;
			std::unique_ptr<Shader> pGeometryShader = nullptr;
		};
	}
}