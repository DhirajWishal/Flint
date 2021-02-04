// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/Pipelines/VulkanPipeline.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanPipeline::ResolveUniformLayouts(const std::vector<ShaderDigest>& shaderDigests)
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