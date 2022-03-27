// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"

namespace Flint
{
	GraphicsPipeline::GraphicsPipeline(Device* pDevice, const std::string& pipelineName, const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget, std::unique_ptr<Shader>&& pVertexShader, std::unique_ptr<Shader>&& pTessellationControlShader, std::unique_ptr<Shader>&& pTessellationEvaluationShader, std::unique_ptr<Shader>&& pGeometryShader, std::unique_ptr<Shader>&& pFragmentShader, const GraphicsPipelineSpecification& specification)
		: Pipeline(pDevice, pipelineName)
		, pRenderTarget(pScreenBoundRenderTarget)
		, pVertexShader(std::move(pVertexShader))
		, pTessellationControlShader(std::move(pTessellationControlShader))
		, pTessellationEvaluationShader(std::move(pTessellationEvaluationShader))
		, pGeometryShader(std::move(pGeometryShader))
		, pFragmentShader(std::move(pFragmentShader))
		, mSpecification(specification)
	{
		if (!pScreenBoundRenderTarget)
			throw std::invalid_argument("Render target pointer should not be null!");

		if (pVertexShader == nullptr)
			throw std::invalid_argument("Vertex shader pointer should not be null!");

		if (pFragmentShader == nullptr)
			throw std::invalid_argument("Fragment shader pointer should not be null!");
	}

	GraphicsPipeline::GraphicsPipeline(Device* pDevice, const std::string& pipelineName, const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget, std::unique_ptr<Shader>&& pVertexShader, std::unique_ptr<Shader>&& pTessellationControlShader, std::unique_ptr<Shader>&& pTessellationEvaluationShader, std::unique_ptr<Shader>&& pGeometryShader, std::unique_ptr<Shader>&& pFragmentShader, const GraphicsPipelineSpecification& specification)
		: Pipeline(pDevice, pipelineName)
		, pRenderTarget(pOffScreenRenderTarget)
		, pVertexShader(std::move(pVertexShader))
		, pTessellationControlShader(std::move(pTessellationControlShader))
		, pTessellationEvaluationShader(std::move(pTessellationEvaluationShader))
		, pGeometryShader(std::move(pGeometryShader))
		, pFragmentShader(std::move(pFragmentShader))
		, mSpecification(specification)
	{
		if (!pOffScreenRenderTarget)
			throw std::invalid_argument("Render target pointer should not be null!");

		if (pVertexShader == nullptr)
			throw std::invalid_argument("Vertex shader pointer should not be null!");
	}
}