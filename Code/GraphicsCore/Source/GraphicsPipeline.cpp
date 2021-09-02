// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"

namespace Flint
{
	namespace Helpers
	{
		std::pair<std::vector<ShaderResourceKey>, std::vector<ShaderResourceKey>> GetResourceKeys(const std::vector<std::shared_ptr<Shader>>& pShaders)
		{
			std::vector<ShaderResourceKey> buffers;
			std::vector<ShaderResourceKey> images;

			for (const auto pShader : pShaders)
			{
				if (!pShader)
					continue;

				const auto resources = pShader->GetShaderResources();
				for (const auto resource : resources)
				{
					if (resource.second == ShaderResourceType::Sampler ||
						resource.second == ShaderResourceType::SampledImage ||
						resource.second == ShaderResourceType::StorageImage ||
						resource.second == ShaderResourceType::CombinedImageSampler)
						INSERT_INTO_VECTOR(images, resource.first);
					else
						INSERT_INTO_VECTOR(buffers, resource.first);
				}
			}

			return { buffers , images };
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
			throw std::invalid_argument("Render target pointer should not be null!");

		if (pVertexShader == nullptr)
			throw std::invalid_argument("Vertex shader pointer should not be null!");

		if (pFragmentShader == nullptr)
			throw std::invalid_argument("Fragment shader pointer should not be null!");
	}

	GraphicsPipeline::GraphicsPipeline(
		const std::shared_ptr<Device>& pDevice,
		const std::string& pipelineName,
		const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget,
		const std::shared_ptr<Shader>& pVertexShader,
		const std::shared_ptr<Shader>& pTessellationControlShader,
		const std::shared_ptr<Shader>& pTessellationEvaluationShader,
		const std::shared_ptr<Shader>& pGeometryShader,
		const std::shared_ptr<Shader>& pFragmentShader,
		const GraphicsPipelineSpecification& specification)
		: Pipeline(pDevice, pipelineName),
		pRenderTarget(pOffScreenRenderTarget),
		pVertexShader(pVertexShader),
		pTessellationControlShader(pTessellationControlShader),
		pTessellationEvaluationShader(pTessellationEvaluationShader),
		pGeometryShader(pGeometryShader),
		pFragmentShader(pFragmentShader),
		mSpecification(specification)
	{
		if (!pOffScreenRenderTarget)
			throw std::invalid_argument("Render target pointer should not be null!");

		if (pVertexShader == nullptr)
			throw std::invalid_argument("Vertex shader pointer should not be null!");
	}

	std::shared_ptr<ResourceMap> GraphicsPipeline::CreateResourceMap() const
	{
		auto [buffers, images] = Helpers::GetResourceKeys({ pVertexShader, pTessellationControlShader, pTessellationEvaluationShader, pGeometryShader, pFragmentShader });
		return std::make_shared<ResourceMap>(buffers, images);
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

	void GraphicsPipeline::ClearDrawData()
	{
		mDrawDataList.clear();
		mDrawDataIndex = 0;
	}
}