// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsPipeline.hpp"
#include "ScreenBoundRenderTarget.hpp"

namespace Flint
{
	namespace _Helpers
	{
		std::vector<std::string> GetBufferResourceNames(const std::shared_ptr<Shader>& pShader)
		{
			if (!pShader)
				return {};

			const auto resources = pShader->GetShaderResources();

			std::vector<std::string> names;
			for (const auto resource : resources)
				if (resource.second.mType != ShaderResourceType::SAMPLER)
					INSERT_INTO_VECTOR(names, resource.first);

			return names;
		}

		std::vector<std::string> GetImageResourceNames(const std::shared_ptr<Shader>& pShader)
		{
			if (!pShader)
				return {};

			const auto resources = pShader->GetShaderResources();

			std::vector<std::string> names;
			for (const auto resource : resources)
				if (resource.second.mType == ShaderResourceType::SAMPLER)
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
		return std::make_shared<ResourceMap>(GetBufferResourceNames(), GetImageResourceNames());
	}

	UI64 GraphicsPipeline::AddDrawData(const std::shared_ptr<ResourceMap>& pResourceMap, const std::shared_ptr<DynamicStateContainer>& pDynamicStates, UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount)
	{
		mDrawDataList[mDrawDataIndex] = DrawData(pResourceMap, pDynamicStates, vertexOffset, vertexCount, indexOffset, indexCount);

		bShouldPrepareResources = true;
		pRenderTarget->FlagAltered();

		return mDrawDataIndex++;
	}

	void GraphicsPipeline::RemoveDrawData(const UI64 drawID)
	{
		mDrawDataList.erase(drawID);

		bShouldPrepareResources = true;
		pRenderTarget->FlagAltered();
	}

	const std::vector<std::string> GraphicsPipeline::GetBufferResourceNames() const
	{
		std::vector<std::string> nameList = _Helpers::GetBufferResourceNames(pVertexShader);

		std::vector<std::string> tempNameList = _Helpers::GetBufferResourceNames(pTessellationControlShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		tempNameList = _Helpers::GetBufferResourceNames(pTessellationEvaluationShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		tempNameList = _Helpers::GetBufferResourceNames(pGeometryShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		tempNameList = _Helpers::GetBufferResourceNames(pFragmentShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		return nameList;
	}

	const std::vector<std::string> GraphicsPipeline::GetImageResourceNames() const
	{
		std::vector<std::string> nameList = _Helpers::GetImageResourceNames(pVertexShader);

		std::vector<std::string> tempNameList = _Helpers::GetImageResourceNames(pTessellationControlShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		tempNameList = _Helpers::GetImageResourceNames(pTessellationEvaluationShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		tempNameList = _Helpers::GetImageResourceNames(pGeometryShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		tempNameList = _Helpers::GetImageResourceNames(pFragmentShader);
		nameList.insert(nameList.end(), tempNameList.begin(), tempNameList.end());

		return nameList;
	}
}