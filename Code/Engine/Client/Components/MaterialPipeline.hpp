// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GraphicsPipeline.hpp"

namespace Flint
{
	namespace Components
	{
		/**
		 * Material pipeline structure.
		 */
		struct MaterialPipeline
		{
			MaterialPipeline(const std::string_view& parent, const std::string_view& name) : mParentController(parent), mComponentName(name) {}

			std::string_view mParentController = "";
			std::string_view mComponentName = "";

			std::shared_ptr<GraphicsPipeline> pPipeline = nullptr;
			std::shared_ptr<Shader> pVertexShader = nullptr;
			std::shared_ptr<Shader> pFragmentShader = nullptr;
			std::shared_ptr<Shader> pTessellationControlShader = nullptr;
			std::shared_ptr<Shader> pTessellationEvaluationShader = nullptr;
			std::shared_ptr<Shader> pGeometryShader = nullptr;
		};
	}
}