// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FPipeline.h"

namespace Flint
{
	/**
	 * Pipeline primitive topology.
	 * This describes how the vertexes are connected.
	 */
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

	/**
	 * Pipeline cull mode.
	 */
	enum class CullMode : UI8 {
		NONE,
		FRONT,
		BACK,
		FRONT_AND_BACK
	};

	/**
	 * Pipeline front face.
	 * This defines how to find the front size of a triangle.
	 */
	enum class FrontFace : UI8 {
		/**
		 * In this mode, the front side of it is facing away from you.
		 */
		COUNTER_CLOCKWISE,

		/**
		 * In this mode, the front is facing towards you.
		 */
		 CLOCKWISE,
	};

	/**
	 * Pipeline polygon mode.
	 * This defines how a vertex group. A vertex group is defined by the primitive topology.
	 */
	enum class PolygonMode : UI8 {
		FILL,
		LINE,
		POINT,
	};

	/**
	 * Pipeline color blend logic.
	 * This defined what logic to apply when color blending.
	 */
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

	/**
	 * Pipeline depth compare logic.
	 * This defines what logic to apply when depth comparing.
	 */
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

	/**
	 * Graphics pipeline specification.
	 * This structure describes how the pipeline should handle certain aspects of it.
	 */
	struct GraphicsPipelineSpecification {
		float mColorBlendConstants[4] = {
			CREATE_COLOR_256(255), CREATE_COLOR_256(255),
			CREATE_COLOR_256(255), CREATE_COLOR_256(255)
		};
		float mDepthBiasFactor = 0.0f;
		float mDepthConstantFactor = 0.0f;
		float mDepthSlopeFactor = 0.0f;
		float mRasterizerLineWidth = 1.0f;
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
	 * Graphics pipelines use rasterization to render graphics to a render target.
	 */
	class FGraphicsPipeline : public FPipeline, public std::enable_shared_from_this<FGraphicsPipeline> {
	public:
		FGraphicsPipeline(std::shared_ptr<FRenderTarget> pRenderTarget, const std::vector<FShaderDigest>& shaderDigests, const GraphicsPipelineSpecification& spec)
			: FPipeline(pRenderTarget, shaderDigests), mSpec(spec) {}
		virtual ~FGraphicsPipeline() {}

		/**
		 * Get the graphics specification.
		 *
		 * @return The const graphics pipeline specification structure.
		 */
		const GraphicsPipelineSpecification GetSpecification() const { return mSpec; }

	protected:
		GraphicsPipelineSpecification mSpec = {};
	};

	namespace GraphicsPipelinePresets
	{
		/**
		 * Create default specification.
		 * This specification can be used with the following,
		 * * Shaders containing 1 input attribute (Vector3 - Position).
		 * * Uniform buffer for MVP (Model View Projection).
		 * * 2 Shader modules (Vertex and Fragment).
		 */
		GraphicsPipelineSpecification CreateDefaultSpec();
	}
}