// Copyright 2021 Dhiraj Wishal
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
			std::shared_ptr<GraphicsPipeline> pPipeline = nullptr;
			std::shared_ptr<Shader> pVertexShader = nullptr;
			std::shared_ptr<Shader> pFragmentShader = nullptr;
			std::shared_ptr<Shader> pTessellationControlShader = nullptr;
			std::shared_ptr<Shader> pTessellationEvaluationShader = nullptr;
			std::shared_ptr<Shader> pGeometryShader = nullptr;
		};
	}
}