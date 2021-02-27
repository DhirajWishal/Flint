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

		enum class ColorBlendLogic : UI8 {
			CLEAR,
			AND,
			AND_REVERSE,
			COPY,
			AND_INVERTED,
			NO_OP,
			XOR,
			OR,
			NOR,
			EQUIVALENT,
			INVERT,
			OR_REVERSE,
			COPY_INVERTED,
			OR_INVERTED,
			NAND,
			SET,
		};

		enum class DepthCompareLogic : UI8 {
			NEVER,
			LESS,
			EQUAL,
			LESS_OR_EQUAL,
			GREATER,
			NOT_EQUAL,
			GREATER_OR_EQUAL,
			ALWAYS,
		};

		struct GraphicsPipelineSpecification {
			float mColorBlendConstants[4] = {
				CREATE_COLOR_256(255), CREATE_COLOR_256(255),
				CREATE_COLOR_256(255), CREATE_COLOR_256(255)
			};
			float mDepthBiasFactor = 0.0f;
			float mDepthConstantFactor = 0.0f;
			float mDepthSlopeFactor = 0.0f;
			float mRasterizerLineWidth = 0.0f;
			float mMinSampleShading = 1.0f;

			PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
			CullMode mCullMode = CullMode::BACK;
			FrontFace mFrontFace = FrontFace::COUNTER_CLOCKWISE;
			PolygonMode mPolygonMode = PolygonMode::FILL;
			ColorBlendLogic mColorBlendLogic = ColorBlendLogic::CLEAR;
			DepthCompareLogic mDepthCompareLogic = DepthCompareLogic::LESS_OR_EQUAL;
			DynamicStateFlags mDynamicStateFlags = DynamicStateFlags(0);
			//DynamicStateFlags mDynamicStateFlags = DynamicStateFlags::VIEWPORT | DynamicStateFlags::SCISSOR;

			bool bEnablePrimitiveRestart = false;
			bool bEnableDepthBias = false;
			bool bEnableDepthClamp = false;
			bool bEnableRasterizerDiscard = false;
			bool bEnableAlphaCoverage = true;
			bool bEnableAlphaToOne = true;
			bool bEnableSampleShading = true;
			bool bEnableColorBlendLogic = true;
			bool bEnableColorBlend = false;
			bool bEnableDepthTest = false;
			bool bEnableDepthWrite = false;
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

	namespace GraphicsPipelinePresets
	{
		/**
		 * Create default specification.
		 * This specification can be used with the following,
		 * * Shaders containing 1 input attribute (Vector3 - Position).
		 * * Uniform buffer for MVP (Model View Projection).
		 * * 2 Shader modules (Vertex and Fragment).
		 */
		Backend::GraphicsPipelineSpecification CreateDefaultSpec();
	}
}