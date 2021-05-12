// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/Pipeline.h"
#include "Core/Backend/RenderTarget.h"
#include "Core/Backend/Image.h"

namespace Flint
{
	namespace Backend
	{
		UniformBufferContainer Pipeline::CreateUniformBuffers() const
		{
			Device* pDevice = pRenderTarget->GetDevice();

			UniformBufferContainer container;
			for (auto itr = mUniformLayouts.begin(); itr != mUniformLayouts.end(); itr++)
				if (itr->second.mType == UniformType::UNIFORM_BUFFER || itr->second.mType == UniformType::UNIFORM_BUFFER_DYNAMIC)
					container[itr->first] = pDevice->CreateBuffer(itr->second.mSize, BufferUsage::UNIFORM, MemoryProfile::TRANSFER_FRIENDLY);

			return container;
		}

		void Pipeline::DestroyUniformBuffers(UniformBufferContainer& uniformBuffers) const
		{
			uniformBuffers.clear();
		}

		UniformImageContainer Pipeline::CreateUniformImages() const
		{
			Device* pDevice = pRenderTarget->GetDevice();

			UniformImageContainer container;
			for (auto itr = mUniformLayouts.begin(); itr != mUniformLayouts.end(); itr++)
			{
				/*if (itr->second.mType == UniformType::STORAGE_IMAGE ||
					itr->second.mType == UniformType::SAMPLER_2D ||
					itr->second.mType == UniformType::SAMPLER_2D_ARRAY ||
					itr->second.mType == UniformType::SAMPLER_CUBE ||
					itr->second.mType == UniformType::SAMPLER_CUBE_ARRAY)
				container[itr->first];	TODO */
			}

			return container;
		}

		void Pipeline::DestroyUniformImages(UniformImageContainer& uniformImages) const
		{
			for (auto& pair : uniformImages)
				pair.second.first->Terminate();

			uniformImages.clear();
		}

		void Pipeline::ResolveUniformLayouts(const std::vector<ShaderDigest>& shaderDigests)
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
	}
}