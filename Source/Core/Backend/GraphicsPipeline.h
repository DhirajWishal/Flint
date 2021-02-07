// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.h"
#include "Core/Macros/Global.h"

#define FLINT_GET_COLOR_255(x)	(x / 255.0f)

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

		enum class DynamicStateFlags : UI8 {
			VIEWPORT = BIT_SHIFT(0),
			SCISSOR = BIT_SHIFT(1),
			LINE_WIDTH = BIT_SHIFT(2),
			DEPTH_BIAS = BIT_SHIFT(3),
			BLEND_CONSTANTS = BIT_SHIFT(4),
			DEPTH_BOUNDS = BIT_SHIFT(5),
		};

		constexpr DynamicStateFlags operator|(const DynamicStateFlags& lhs, const DynamicStateFlags& rhs)
		{
			return static_cast<DynamicStateFlags>(static_cast<UI8>(lhs) | static_cast<UI8>(rhs));
		}

		constexpr bool operator&(const DynamicStateFlags& lhs, const DynamicStateFlags& rhs)
		{
			return static_cast<UI8>(lhs) & static_cast<UI8>(rhs);
		}

		struct GraphicsPipelineSpecification {
			float mColorBlendConstants[4] = {
				FLINT_GET_COLOR_255(255), FLINT_GET_COLOR_255(255),
				FLINT_GET_COLOR_255(255), FLINT_GET_COLOR_255(255)
			};
			float mDepthBiasFactor = 0.0f;
			float mDepthConstantFactor = 0.0f;
			float mDepthSlopeFactor = 0.0f;
			float mRasterizerLineWidth = 0.0f;
			float mMinSampleShading = 1.0f;

			PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::LINE_LIST;
			CullMode mCullMode = CullMode::BACK;
			FrontFace mFrontFace = FrontFace::COUNTER_CLOCKWISE;
			PolygonMode mPolygonMode = PolygonMode::FILL;
			ColorBlendLogic mColorBlendLogic = ColorBlendLogic::CLEAR;
			DepthCompareLogic mDepthCompareLogic = DepthCompareLogic::LESS_OR_EQUAL;
			DynamicStateFlags mDynamicStateFlags = DynamicStateFlags::VIEWPORT | DynamicStateFlags::SCISSOR;

			bool bEnablePrimitiveRestart = false;
			bool bEnableDepthBias = false;
			bool bEnableDepthClamp = false;
			bool bEnableRasterizerDiscard = false;
			bool bEnableAlphaCoverage = true;
			bool bEnableAlphaToOne = true;
			bool bEnableSampleShading = true;
			bool bEnableColorBlendLogic = true;
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
}