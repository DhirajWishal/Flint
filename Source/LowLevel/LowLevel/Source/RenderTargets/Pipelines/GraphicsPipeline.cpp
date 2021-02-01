// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/RenderTargets/Pipelines/GraphicsPipeline.h"
#include "Core/Backend/Interface/ScreenBoundRenderTarget.h"
#include "Core/Backend/Interface/Resources.h"

namespace Flint
{
	namespace LowLevel
	{
		void GraphicsPipeline::Initialize(const RenderTargetSB2D& renderTarget, const std::vector<ShaderCode>& shaders, const GraphicsPipelineSpecification& spec)
		{
			std::vector<ShaderDigest> digests;
			for (auto itr = shaders.begin(); itr != shaders.end(); itr++)
				INSERT_INTO_VECTOR(digests, itr->Digest());

			mHandle = Interface::CreateGraphicsPipelineSB2D(renderTarget.GetHandle(), digests, spec);
		}

		void GraphicsPipeline::Initialize(const RenderTargetSB3D& renderTarget, const std::vector<ShaderCode>& shaders, const GraphicsPipelineSpecification& spec)
		{
			std::vector<ShaderDigest> digests;
			for (auto itr = shaders.begin(); itr != shaders.end(); itr++)
				INSERT_INTO_VECTOR(digests, itr->Digest());

			mHandle = Interface::CreateGraphicsPipelineSB3D(renderTarget.GetHandle(), digests, spec);
		}

		void GraphicsPipeline::Terminate()
		{
			Interface::DestroyGraphicsPipeline(mHandle);
		}
	}
}