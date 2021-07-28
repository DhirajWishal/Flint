// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsPipeline.hpp"

namespace Flint
{
	GraphicsPipeline::GraphicsPipeline(
		const std::shared_ptr<Device>& pDevice,
		const std::shared_ptr<RenderTarget>& pRenderTarget,
		const std::shared_ptr<Shader>& pVertexShader,
		const std::shared_ptr<Shader>& pTessellationControlShader,
		const std::shared_ptr<Shader>& pTessellationEvaluationShader,
		const std::shared_ptr<Shader>& pGeometryShader,
		const std::shared_ptr<Shader>& pFragmentShader)
		: Pipeline(pDevice),
		pRenderTarget(pRenderTarget),
		pVertexShader(pVertexShader),
		pFragmentShader(pFragmentShader),
		pTessellationControlShader(pTessellationControlShader),
		pTessellationEvaluationShader(pTessellationEvaluationShader),
		pGeometryShader(pGeometryShader)
	{
		if (!pRenderTarget)
			FLINT_THROW_INVALID_ARGUMENT("Render target pointer should not be null!");

		if (pVertexShader == nullptr)
			FLINT_THROW_INVALID_ARGUMENT("Vertex shader pointer should not be null!");

		if (pFragmentShader == nullptr)
			FLINT_THROW_INVALID_ARGUMENT("Fragment shader pointer should not be null!");
	}
}