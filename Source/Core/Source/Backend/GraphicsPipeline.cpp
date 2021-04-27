// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/GraphicsPipeline.h"
#include "Core\Backend\Image.h"
#include "Core\Backend\Buffer.h"
#include "Core\Backend\RenderTarget.h"

namespace Flint
{
	UniformBufferContainer FPipeline::CreateUniformBuffers() const
	{
		FDevice* pDevice = pRenderTarget->GetDevice();

		UniformBufferContainer container;
		for (auto itr = mUniformLayouts.begin(); itr != mUniformLayouts.end(); itr++)
			if (itr->second.mType == UniformType::UNIFORM_BUFFER || itr->second.mType == UniformType::UNIFORM_BUFFER_DYNAMIC)
				container[itr->first] = pDevice->CreateBuffer(itr->second.mSize, BufferUsage::UNIFORM, MemoryProfile::TRANSFER_FRIENDLY);

		return container;
	}

	void FPipeline::DestroyUniformBuffers(UniformBufferContainer& uniformBuffers) const
	{
		for (auto bufferPair : uniformBuffers)
			bufferPair.second->Terminate();

		uniformBuffers.clear();
	}

	UniformImageContainer FPipeline::CreateUniformImages() const
	{
		FDevice* pDevice = pRenderTarget->GetDevice();

		UniformImageContainer container;
		for (auto itr = mUniformLayouts.begin(); itr != mUniformLayouts.end(); itr++)
		{
			if (itr->second.mType == UniformType::STORAGE_IMAGE ||
				itr->second.mType == UniformType::SAMPLER_2D ||
				itr->second.mType == UniformType::SAMPLER_2D_ARRAY ||
				itr->second.mType == UniformType::SAMPLER_CUBE ||
				itr->second.mType == UniformType::SAMPLER_CUBE_ARRAY)
				container[itr->first] = {};
		}

		return container;
	}


	void FPipeline::DestroyUniformImages(UniformImageContainer& uniformImages) const
	{
		for (auto imagePair : uniformImages)
			imagePair.second.first->Terminate();

		uniformImages.clear();
	}


	void FPipeline::ResolveUniformLayouts(const std::vector<ShaderDigest>& shaderDigests)
	{
		for (auto itr = shaderDigests.begin(); itr != shaderDigests.end(); itr++)
		{
			for (auto pUniform = itr->mUniforms.begin(); pUniform != itr->mUniforms.end(); pUniform++)
			{
				if (pUniform->mType == UniformType::UNIFORM_BUFFER ||
					pUniform->mType == UniformType::UNIFORM_BUFFER_DYNAMIC ||
					pUniform->mType == UniformType::STORAGE_BUFFER ||
					pUniform->mType == UniformType::STORAGE_BUFFER_DYNAMIC)
					mUniformLayouts[pUniform->mName] = *pUniform;
			}
		}
	}

	namespace GraphicsPipelinePresets
	{
		GraphicsPipelineSpecification CreateDefaultSpec()
		{
			GraphicsPipelineSpecification spec = {};
			//spec.bEnableSampleShading = false;
			//spec.bEnableAlphaCoverage = false;
			//spec.bEnableAlphaToOne = false;
			//spec.bEnableColorBlendLogic = false;
			//spec.bEnableDepthTest = true;
			//spec.bEnableDepthWrite = true;
			//spec.mRasterizerLineWidth = 1.0f;
			//spec.mMinSampleShading = 0.0f;
			//spec.mColorBlendConstants[0] = 0.0f;
			//spec.mColorBlendConstants[1] = 0.0f;
			//spec.mColorBlendConstants[2] = 0.0f;
			//spec.mColorBlendConstants[3] = 0.0f;

			return spec;
		}
	}
}