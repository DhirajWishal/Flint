// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"

namespace Flint
{
	namespace Helpers
	{
		std::pair<boost::container::vector<std::string>, boost::container::vector<std::string>> GetResourceNames(const boost::container::vector<boost::shared_ptr<Shader>>& pShaders)
		{
			boost::container::vector<std::string> buffers;
			boost::container::vector<std::string> images;

			for (const auto pShader : pShaders)
			{
				if (!pShader)
					continue;

				const auto resources = pShader->GetShaderResources();
				for (const auto resource : resources)
				{
					if (resource.second.mType == ShaderResourceType::SAMPLER ||
						resource.second.mType == ShaderResourceType::SAMPLED_IMAGE ||
						resource.second.mType == ShaderResourceType::STORAGE_IMAGE ||
						resource.second.mType == ShaderResourceType::COMBINED_IMAGE_SAMPLER)
						INSERT_INTO_VECTOR(images, resource.first);
					else
						INSERT_INTO_VECTOR(buffers, resource.first);
				}
			}

			return { buffers , images };
		}
	}

	GraphicsPipeline::GraphicsPipeline(
		const boost::shared_ptr<Device>& pDevice,
		const std::string& pipelineName,
		const boost::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
		const boost::shared_ptr<Shader>& pVertexShader,
		const boost::shared_ptr<Shader>& pTessellationControlShader,
		const boost::shared_ptr<Shader>& pTessellationEvaluationShader,
		const boost::shared_ptr<Shader>& pGeometryShader,
		const boost::shared_ptr<Shader>& pFragmentShader,
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

	GraphicsPipeline::GraphicsPipeline(
		const boost::shared_ptr<Device>& pDevice,
		const std::string& pipelineName,
		const boost::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget,
		const boost::shared_ptr<Shader>& pVertexShader,
		const boost::shared_ptr<Shader>& pTessellationControlShader,
		const boost::shared_ptr<Shader>& pTessellationEvaluationShader,
		const boost::shared_ptr<Shader>& pGeometryShader,
		const boost::shared_ptr<Shader>& pFragmentShader,
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
			FLINT_THROW_INVALID_ARGUMENT("Render target pointer should not be null!");

		if (pVertexShader == nullptr)
			FLINT_THROW_INVALID_ARGUMENT("Vertex shader pointer should not be null!");
	}

	boost::shared_ptr<ResourceMap> GraphicsPipeline::CreateResourceMap() const
	{
		auto [buffers, images] = Helpers::GetResourceNames({ pVertexShader, pTessellationControlShader, pTessellationEvaluationShader, pGeometryShader, pFragmentShader });
		return boost::make_shared<ResourceMap>(buffers, images);
	}

	UI64 GraphicsPipeline::AddDrawData(const boost::shared_ptr<ResourceMap>& pResourceMap, const boost::shared_ptr<DynamicStateContainer>& pDynamicStates, UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount)
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