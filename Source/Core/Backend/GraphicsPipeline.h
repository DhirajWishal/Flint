// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.h"

namespace Flint
{
	namespace Backend
	{
		enum class PrimitiveTopology : UI8 {
			POINT_LIST,
			LINE_LIST,
			LINE_STRIP,
			TRIANGLE_LIST,
			TRIANGLE_STRIP,
			TRIANGLE_FAN,
			LINE_LIST_WITH_ADJACENCY,
			LINE_STRIP_WITH_ADJACENCY,
			TRIANGLE_LIST_WITH_ADJACENCY,
			TRIANGLE_STRIP_WITH_ADJACENCY,
			PATCH_LIST,
		};

		enum class CullMode : UI8 {
			NONE,
			FRONT,
			BACK,
			FRONT_AND_BACK
		};

		enum class FrontFace : UI8 {
			COUNTER_CLOCKWISE,
			CLOCKWISE,
		};

		enum class PolygonMode : UI8 {
			FILL,
			LINE,
			POINT,
		};

		struct GraphicsPipelineSpecification {
			bool bEnablePrimitiveRestart = false;
			PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::LINE_LIST;

			CullMode mCullMode = CullMode::BACK;
			float mDepthBiasFactor = 0.0f;
			float mDepthConstantFactor = 0.0f;
			float mDepthSlopeFactor = 0.0f;
			FrontFace mFrontFace = FrontFace::COUNTER_CLOCKWISE;
			float mRasterizerLineWidth = 0.0f;
			PolygonMode mPolygonMode = PolygonMode::FILL;

			bool bEnableDepthBias = false;
			bool bEnableDepthClamp = false;
			bool bEnableRasterizerDiscard = false;
		};

		/**
		 * Graphics Pipeline object.
		 */
		class GraphicsPipeline : public Pipeline {
		public:
			GraphicsPipeline() {}

			/**
			 * Initialize the pipeline.
			 *
			 * @param pRenderTarget: The render target that the pipeline is bound to.
			 * @param shaderDigest: The shader digests to use.
			 */
			virtual void Initialize(RenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigests, const GraphicsPipelineSpecification& spec) = 0;

		protected:
			GraphicsPipelineSpecification mSpec = {};
		};
	}
}