// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/Pipeline.h"
#include "Core/Backend/RenderTarget.h"

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

		DynamicStateContainer::DynamicStateContainer(const DynamicStateContainer& other)
			: pDynamicStates(other.pDynamicStates)
		{
		}

		DynamicStateContainer::DynamicStateContainer(DynamicStateContainer&& other) noexcept
			: pDynamicStates(std::move(other.pDynamicStates))
		{
		}

		DynamicStateContainer::~DynamicStateContainer()
		{
		}

		void DynamicStateContainer::AddViewPort(const Vector2& extent, const Vector2& depth, const Vector2& offset)
		{
			INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<ViewPort>(extent, offset, depth));
		}

		void DynamicStateContainer::AddScissor(const Vector2& extent, const Vector2& offset)
		{
			INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<Scissor>(extent, offset));
		}

		void DynamicStateContainer::AddLineWidth(const float width)
		{
			INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<LineWidth>(width));
		}

		void DynamicStateContainer::AddDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor)
		{
			INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<DepthBias>(biasFactor, clampFactor, slopeFactor));
		}

		void DynamicStateContainer::AddBlendConstants(const float(&constants)[4])
		{
			INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<BlendConstants>(constants));
		}

		void DynamicStateContainer::AddDepthBounds(const Vector2& bounds)
		{
			INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<DepthBounds>(bounds));
		}

		DynamicStateContainer& DynamicStateContainer::operator=(const DynamicStateContainer& other)
		{
			pDynamicStates = other.pDynamicStates;
			return *this;
		}

		DynamicStateContainer& DynamicStateContainer::operator=(DynamicStateContainer&& other) noexcept
		{
			pDynamicStates = std::move(other.pDynamicStates);
			return *this;
		}
	}
}