// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Shader.hpp"
#include "RenderTarget.hpp"
#include "DynamicStateContainer.hpp"
#include "ResourceMap.hpp"

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
	 * Color blend factor enum.
	 */
	enum class ColorBlendFactor : UI8 {
		ZERO,
		ONE,
		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,
		CONSTANT_COLOR,
		ONE_MINUS_CONSTANT_COLOR,
		CONSTANT_ALPHA,
		ONE_MINUS_CONSTANT_ALPHA,
		SRC_ALPHA_SATURATE,
		SRC1_COLOR,
		ONE_MINUS_SRC1_COLOR,
		SRC1_ALPHA,
		ONE_MINUS_SRC1_ALPHA,
	};

	/**
	 * Color blend operator enum.
	 */
	enum class ColorBlendOperator : UI8 {
		ADD,
		SUBTRACT,
		REVERSE_SUBTRACT,
		MIN,
		MAX,
		ZERO,
		SRC,
		DST,
		SRC_OVER,
		DST_OVER,
		SRC_IN,
		DST_IN,
		SRC_OUT,
		DST_OUT,
		SRC_ATOP,
		DST_ATOP,
		XOR,
		MULTIPLY,
		SCREEN,
		OVERLAY,
		DARKEN,
		LIGHTEN,
		COLORDODGE,
		COLORBURN,
		HARDLIGHT,
		SOFTLIGHT,
		DIFFERENCE,
		EXCLUSION,
		INVERT,
		INVERT_RGB,
		LINEARDODGE,
		LINEARBURN,
		VIVIDLIGHT,
		LINEARLIGHT,
		PINLIGHT,
		HARDMIX,
		HSL_HUE,
		HSL_SATURATION,
		HSL_COLOR,
		HSL_LUMINOSITY,
		PLUS,
		PLUS_CLAMPED,
		PLUS_CLAMPED_ALPHA,
		PLUS_DARKER,
		MINUS,
		MINUS_CLAMPED,
		CONTRAST,
		INVERT_OVG,
		RED,
		GREEN,
		BLUE,
	};

	/**
	 * Color write mask enum.
	 */
	enum class ColorWriteMask : UI8 {
		R = BIT_SHIFT(0),
		G = BIT_SHIFT(1),
		B = BIT_SHIFT(2),
		A = BIT_SHIFT(3),
	};

	/**
	 * Color write mask bitwise OR operator.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The OR performed color write mask.
	 */
	constexpr ColorWriteMask operator|(const ColorWriteMask& lhs, const ColorWriteMask& rhs) { return static_cast<ColorWriteMask>(static_cast<UI8>(lhs) | static_cast<UI8>(rhs)); }

	/**
	 * Color write mask bitwise AND operator.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The AND performed color write mask.
	 */
	constexpr ColorWriteMask operator&(const ColorWriteMask& lhs, const ColorWriteMask& rhs) { return static_cast<ColorWriteMask>(static_cast<UI8>(lhs) & static_cast<UI8>(rhs)); }

	/**
	 * Color blend attachment structure.
	 */
	struct ColorBlendAttachment
	{
		bool mEnableBlend = false;
		ColorBlendFactor mSrcBlendFactor = ColorBlendFactor::ZERO;
		ColorBlendFactor mDstBlendFactor = ColorBlendFactor::ZERO;
		ColorBlendFactor mSrcAlphaBlendFactor = ColorBlendFactor::ZERO;
		ColorBlendFactor mDstAlphaBlendFactor = ColorBlendFactor::ZERO;
		ColorBlendOperator mBlendOperator = ColorBlendOperator::ADD;
		ColorBlendOperator mAlphaBlendOperator = ColorBlendOperator::ADD;
		ColorWriteMask mColorWriteMask = ColorWriteMask::R | ColorWriteMask::G | ColorWriteMask::B | ColorWriteMask::A;
	};

	/**
	 * Graphics pipeline specification.
	 * This structure describes how the pipeline should handle certain aspects of it.
	 */
	struct GraphicsPipelineSpecification {
		GraphicsPipelineSpecification() = default;

		std::vector<ColorBlendAttachment> mColorBlendAttachments = { ColorBlendAttachment() };

		float mColorBlendConstants[4] = {
			CREATE_COLOR_256(0), CREATE_COLOR_256(0),
			CREATE_COLOR_256(0), CREATE_COLOR_256(0)
		};
		float mDepthBiasFactor = 0.0f;
		float mDepthConstantFactor = 0.0f;
		float mDepthSlopeFactor = 0.0f;
		float mRasterizerLineWidth = 1.0f;
		float mMinSampleShading = 1.0f;

		UI32 mTessellationPatchControlPoints = 0;

		PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		CullMode mCullMode = CullMode::BACK;
		FrontFace mFrontFace = FrontFace::COUNTER_CLOCKWISE;
		PolygonMode mPolygonMode = PolygonMode::FILL;
		ColorBlendLogic mColorBlendLogic = ColorBlendLogic::CLEAR;
		DepthCompareLogic mDepthCompareLogic = DepthCompareLogic::LESS_OR_EQUAL;
		DynamicStateFlags mDynamicStateFlags = DynamicStateFlags(0);
		RasterizationSamples mRasterizationSamples = RasterizationSamples::BITS_1;

		bool bEnablePrimitiveRestart = false;
		bool bEnableDepthBias = false;
		bool bEnableDepthClamp = false;
		bool bEnableRasterizerDiscard = false;
		bool bEnableAlphaCoverage = false;
		bool bEnableAlphaToOne = false;
		bool bEnableSampleShading = true;
		bool bEnableColorBlendLogic = false;
		bool bEnableDepthTest = true;
		bool bEnableDepthWrite = true;
	};

	/**
	 * Flint graphics pipeline.
	 * This object uses rasterization to draw graphics.
	 *
	 * Graphics pipelines have 5 main stages, with 3 optional stages.
	 * 1. Vertex.
	 * 2. Tessellation control (optional).
	 * 3. Tessellation evaluation (optional).
	 * 4. Geometry (optional).
	 * 5. Fragment (optional).
	 */
	class GraphicsPipeline : public Pipeline
	{
	protected:
		struct DrawData
		{
			DrawData() = default;
			DrawData(
				const std::shared_ptr<ResourceMap>& pResourceMap,
				const std::shared_ptr<DynamicStateContainer>& pDynamicStates,
				UI64 vertexOffset, UI64 vertexCount,
				UI64 indexOffset, UI64 indexCount)
				: pResourceMap(pResourceMap), pDynamicStates(pDynamicStates),
				mVertexOffset(vertexOffset), mVertexCount(vertexCount),
				mIndexOffset(indexOffset), mIndexCount(indexCount) {}

			UI64 mVertexOffset = 0, mVertexCount = 0;
			UI64 mIndexOffset = 0, mIndexCount = 0;

			std::shared_ptr<ResourceMap> pResourceMap = nullptr;
			std::shared_ptr<DynamicStateContainer> pDynamicStates = nullptr;
		};

	public:
		/**
		 * Construct the pipeline using a screen bound render target.
		 *
		 * @param pDevice: The device pointer.
		 * @param pipelineName: The name of the pipeline.
		 * @param pScreenBoundRenderTarget: The screen bound render target pointer.
		 * @param pVertexShader: The vertex shader pointer.
		 * @param pTessellationControlShader: The tessellation control shader (optional).
		 * @param pTessellationEvaluationShader: The tessellation evaluation shader (optional).
		 * @param pGeometryShader: The geometry shader (optional).
		 * @param pFragmentShader: The fragment shader (optional).
		 * @param specification: The pipeline specification.
		 */
		GraphicsPipeline(
			const std::shared_ptr<Device>& pDevice,
			const std::string& pipelineName,
			const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
			const std::shared_ptr<Shader>& pVertexShader,
			const std::shared_ptr<Shader>& pTessellationControlShader,
			const std::shared_ptr<Shader>& pTessellationEvaluationShader,
			const std::shared_ptr<Shader>& pGeometryShader,
			const std::shared_ptr<Shader>& pFragmentShader,
			const GraphicsPipelineSpecification& specification);

		/**
		 * Construct the pipeline using an off screen render target.
		 *
		 * @param pDevice: The device pointer.
		 * @param pipelineName: The name of the pipeline.
		 * @param pOffScreenRenderTarget: The off screen render target pointer.
		 * @param pVertexShader: The vertex shader pointer.
		 * @param pTessellationControlShader: The tessellation control shader (optional).
		 * @param pTessellationEvaluationShader: The tessellation evaluation shader (optional).
		 * @param pGeometryShader: The geometry shader (optional).
		 * @param pFragmentShader: The fragment shader (optional).
		 * @param specification: The pipeline specification.
		 */
		GraphicsPipeline(
			const std::shared_ptr<Device>& pDevice,
			const std::string& pipelineName,
			const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget,
			const std::shared_ptr<Shader>& pVertexShader,
			const std::shared_ptr<Shader>& pTessellationControlShader,
			const std::shared_ptr<Shader>& pTessellationEvaluationShader,
			const std::shared_ptr<Shader>& pGeometryShader,
			const std::shared_ptr<Shader>& pFragmentShader,
			const GraphicsPipelineSpecification& specification);

		/**
		 * Recreate the graphics pipeline.
		 * This is needed once the render target is resized.
		 *
		 * @param pScreenBoundRenderTarget: The screen bound render target pointer.
		 */
		virtual void Recreate(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget) = 0;

		/**
		 * Get the current graphics specification of the pipeline.
		 *
		 * @return The graphics specification.
		 */
		const GraphicsPipelineSpecification GetSpecification() const { return mSpecification; }

		/**
		 * Create a new resource map.
		 *
		 * @return The newly created resource map.
		 */
		std::shared_ptr<ResourceMap> CreateResourceMap() const;

		/**
		 * Get draw data from the pipeline.
		 *
		 * @return The draw data array.
		 */
		const std::unordered_map<UI64, DrawData> GetDrawData() const { return mDrawDataList; }

		/**
		 * Get the number of draw data stored.
		 *
		 * @return The draw data count.
		 */
		const UI64 GetDrawDataCount() const { return mDrawDataList.size(); }

		/**
		 * Get the current draw data index.
		 *
		 * @return The index.
		 */
		const UI64 GetCurrentDrawIndex() const { return mDrawDataIndex; }

		/**
		 * Add draw data to draw.
		 *
		 * @param pResourceMap: The resource map to bind with.
		 * @param pDynamicStates: The dynamic states of the draw.
		 * @param vertexOffset: The vertex offset to begin.
		 * @param vertexCount: The number of vertexes to draw.
		 * @param indexOffset: The index offset to begin.
		 * @param indexCount: The number of index to draw.
		 * @return The draw ID.
		 */
		UI64 AddDrawData(const std::shared_ptr<ResourceMap>& pResourceMap, const std::shared_ptr<DynamicStateContainer>& pDynamicStates, UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount);

		/**
		 * Remove a draw data from the pipeline.
		 *
		 * @param drawID: The draw ID of the draw data.
		 */
		void RemoveDrawData(const UI64 drawID);

		/**
		 * Clear all the draw data.
		 */
		void ClearDrawData();

	protected:
		GraphicsPipelineSpecification mSpecification = {};

		std::unordered_map<UI64, DrawData> mDrawDataList = {};
		UI64 mDrawDataIndex = 0;

		std::shared_ptr<RenderTarget> pRenderTarget = nullptr;

		std::shared_ptr<Shader> pVertexShader = nullptr;
		std::shared_ptr<Shader> pFragmentShader = nullptr;
		std::shared_ptr<Shader> pTessellationControlShader = nullptr;
		std::shared_ptr<Shader> pTessellationEvaluationShader = nullptr;
		std::shared_ptr<Shader> pGeometryShader = nullptr;
	};
}