// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "LowLevel/RenderTargets/RenderTargetSB2D.h"
#include "LowLevel/RenderTargets/RenderTargetSB3D.h"
#include "LowLevel/ShaderCode.h"

namespace Flint
{
	namespace LowLevel
	{
		/**
		 * Flint Graphics Pipeline object.
		 * This pipeline uses rasterization to render to a render target.
		 */
		class GraphicsPipeline {
		public:
			GraphicsPipeline() {}
			~GraphicsPipeline() {}

			void Initialize(const RenderTargetSB2D& renderTarget, const std::vector<ShaderCode>& shaders, const GraphicsPipelineSpecification& spec = {});
			void Initialize(const RenderTargetSB3D& renderTarget, const std::vector<ShaderCode>& shaders, const GraphicsPipelineSpecification& spec = {});
			void Terminate();

		public:
			Interface::GraphicsPipelineHandle mHandle = FLINT_NULL_HANDLE;
		};
	}
}