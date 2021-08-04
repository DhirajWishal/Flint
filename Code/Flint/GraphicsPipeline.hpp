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
	 * Graphics pipeline specification.
	 * This structure describes how the pipeline should handle certain aspects of it.
	 */
	struct GraphicsPipelineSpecification {
		GraphicsPipelineSpecification() = default;

		float mColorBlendConstants[4] = {
			CREATE_COLOR_256(255), CREATE_COLOR_256(255),
			CREATE_COLOR_256(255), CREATE_COLOR_256(255)
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

		bool bEnablePrimitiveRestart = false;
		bool bEnableDepthBias = false;
		bool bEnableDepthClamp = false;
		bool bEnableRasterizerDiscard = false;
		bool bEnableAlphaCoverage = false;
		bool bEnableAlphaToOne = false;
		bool bEnableSampleShading = true;
		bool bEnableColorBlendLogic = false;
		bool bEnableColorBlend = false;
		bool bEnableDepthTest = false;
		bool bEnableDepthWrite = false;
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
	 * 5. Fragment.
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
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 * @param pipelineName: The name of the pipeline.
		 * @param pScreenBoundRenderTarget: The screen bound render target pointer.
		 * @param pVertexShader: The vertex shader pointer.
		 * @param pTessellationControlShader: The tessellation control shader (optional).
		 * @param pTessellationEvaluationShader: The tessellation evaluation shader (optional).
		 * @param pGeometryShader: The geometry shader (optional).
		 * @param pFragmentShader: The fragment shader.
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

		bool bShouldPrepareResources = true;
	};
}