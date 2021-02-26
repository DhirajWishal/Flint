// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/Pipeline.h"

namespace Flint
{
	namespace Backend
	{
		UniformBufferContainer Pipeline::CreateUniformBuffers() const
		{
			Backend::Device* pDevice = pRenderTarget->GetDevice();

			Backend::UniformBufferContainer container;
			for (auto itr = mUniformLayouts.begin(); itr != mUniformLayouts.end(); itr++)
			{
				if (itr->second.mType == UniformType::UNIFORM_BUFFER || itr->second.mType == UniformType::UNIFORM_BUFFER_DYNAMIC)
				{
					Backend::Buffer* pBuffer = pDevice->CreateBuffer(itr->second.mSize, Backend::BufferUsage::UNIFORM, Backend::MemoryProfile::TRANSFER_FRIENDLY);
					container[itr->first] = pBuffer;
				}
			}

			return container;
		}

		void Pipeline::DestroyUniformBuffers(UniformBufferContainer& uniformBuffers) const
		{
			for (auto itr = uniformBuffers.begin(); itr != uniformBuffers.end(); itr++)
			{
				itr->second->Terminate();
				delete itr->second;
			}

			uniformBuffers.clear();
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