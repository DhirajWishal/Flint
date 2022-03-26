// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"

namespace Flint
{
	GraphicsPipeline::GraphicsPipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pTessellationControlShader, const std::shared_ptr<Shader>& pTessellationEvaluationShader, const std::shared_ptr<Shader>& pGeometryShader, const std::shared_ptr<Shader>& pFragmentShader, const GraphicsPipelineSpecification& specification)
		: Pipeline(pDevice, pipelineName), pRenderTarget(pScreenBoundRenderTarget), pVertexShader(pVertexShader), pTessellationControlShader(pTessellationControlShader), pTessellationEvaluationShader(pTessellationEvaluationShader), pGeometryShader(pGeometryShader), pFragmentShader(pFragmentShader), mSpecification(specification)
	{
		if (!pScreenBoundRenderTarget)
			throw std::invalid_argument("Render target pointer should not be null!");

		if (pVertexShader == nullptr)
			throw std::invalid_argument("Vertex shader pointer should not be null!");

		if (pFragmentShader == nullptr)
			throw std::invalid_argument("Fragment shader pointer should not be null!");
	}

	GraphicsPipeline::GraphicsPipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pTessellationControlShader, const std::shared_ptr<Shader>& pTessellationEvaluationShader, const std::shared_ptr<Shader>& pGeometryShader, const std::shared_ptr<Shader>& pFragmentShader, const GraphicsPipelineSpecification& specification)
		: Pipeline(pDevice, pipelineName), pRenderTarget(pOffScreenRenderTarget), pVertexShader(pVertexShader), pTessellationControlShader(pTessellationControlShader), pTessellationEvaluationShader(pTessellationEvaluationShader), pGeometryShader(pGeometryShader), pFragmentShader(pFragmentShader), mSpecification(specification)
	{
		if (!pOffScreenRenderTarget)
			throw std::invalid_argument("Render target pointer should not be null!");

		if (pVertexShader == nullptr)
			throw std::invalid_argument("Vertex shader pointer should not be null!");
	}
}