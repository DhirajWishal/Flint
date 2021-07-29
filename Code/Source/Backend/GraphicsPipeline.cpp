// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsPipeline.hpp"
#include "ScreenBoundRenderTarget.hpp"

namespace Flint
{
	namespace _Helpers
	{
		std::vector<std::string> GetResourceNames(const std::shared_ptr<Shader>& pShader)
		{
			if (!pShader)
				return {};

			std::vector<std::string> names;
			const auto resources = pShader->GetShaderResources();

			for (const auto resource : resources)
				INSERT_INTO_VECTOR(names, resource.first);

			return names;
		}
	}

	GraphicsPipeline::GraphicsPipeline(
		const std::shared_ptr<Device>& pDevice,
		const std::string& pipelineName,
		const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
		const std::shared_ptr<Shader>& pVertexShader,
		const std::shared_ptr<Shader>& pTessellationControlShader,
		const std::shared_ptr<Shader>& pTessellationEvaluationShader,
		const std::shared_ptr<Shader>& pGeometryShader,
		const std::shared_ptr<Shader>& pFragmentShader,
		const GraphicsPipelineSpecification& specification)
		: Pipeline(pDevice, pipelineName),
		pRenderTarget(pScreenBoundRenderTarget),
		pVertexShader(pVertexShader),
		pTessellationControlShader(pTessellationControlShader),
		pTessellationEvaluationShader(pTessellationEvaluationShader),
		pGeometryShader(pGeometryShader),
		pFragmentShader(pFragmentShader),
		mSpecification(specification)
	{
		if (!pScreenBoundRenderTarget)
			FLINT_THROW_INVALID_ARGUMENT("Render target pointer should not be null!");

		if (pVertexShader == nullptr)
			FLINT_THROW_INVALID_ARGUMENT("Vertex shader pointer should not be null!");

		if (pFragmentShader == nullptr)
			FLINT_THROW_INVALID_ARGUMENT("Fragment shader pointer should not be null!");
	}

	std::shared_ptr<ResourceMap> GraphicsPipeline::CreateResourceMap() const
	{
		return std::make_shared<ResourceMap>(GetResourceNames());
	}

	void GraphicsPipeline::AddDrawData(const std::shared_ptr<ResourceMap>& pResourceMap, const std::shared_ptr<DynamicStateContainer>& pDynamicStates, UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount)
	{
		INSERT_INTO_VECTOR(mDrawDataList, DrawData(pResourceMap, pDynamicStates, vertexOffset, vertexCount, indexOffset, indexCount));
	}

	const std::vector<std::string> GraphicsPipeline::GetResourceNames() const
	{
		std::vector<std::string> nameList = _Helpers::GetResourceNames(pVertexShader);

		std::vector<std::string> tempNameList = _Helpers::GetResourceNames(pTessellationControlShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		tempNameList = _Helpers::GetResourceNames(pTessellationEvaluationShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		tempNameList = _Helpers::GetResourceNames(pGeometryShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		tempNameList = _Helpers::GetResourceNames(pFragmentShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		return nameList;
	}
}