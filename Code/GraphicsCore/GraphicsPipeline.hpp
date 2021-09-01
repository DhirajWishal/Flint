// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Shader.hpp"
#include "RenderTarget.hpp"
#include "DynamicStateContainer.hpp"

namespace Flint
{
	/**
	 * Pipeline primitive topology.
	 * This describes how the vertexes are connected.
	 */
	enum class PrimitiveTopology : UI8 {
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		TriangleFan,
		LineListWithAdjacency,
		LineStripWithAdjacency,
		TriangleListWithAdjacency,
		TriangleStripWithAdjacency,
		PatchList,
	};

	/**
	 * Pipeline cull mode.
	 */
	enum class CullMode : UI8 {
		None,
		Front,
		Back,
		FrontAndBack
	};

	/**
	 * Pipeline front face.
	 * This defines how to find the front size of a triangle.
	 */
	enum class FrontFace : UI8 {
		/**
		 * In this mode, the front side of it is facing away from you.
		 */
		CounterClockwise,

		/**
		 * In this mode, the front is facing towards you.
		 */
		 Clockwise,
	};

	/**
	 * Pipeline polygon mode.
	 * This defines how a vertex group. A vertex group is defined by the primitive topology.
	 */
	enum class PolygonMode : UI8 {
		Fill,
		Line,
		Point,
	};

	/**
	 * Pipeline color blend logic.
	 * This defined what logic to apply when color blending.
	 */
	enum class ColorBlendLogic : UI8 {
		Clear,
		And,
		AndReverse,
		Copy,
		AndInverted,
		NoOperator,
		XOR,
		OR,
		NOR,
		Equivalent,
		Invert,
		ReverseOR,
		CopyInverted,
		InvertedOR,
		NAND,
		Set,
	};

	/**
	 * Pipeline depth compare logic.
	 * This defines what logic to apply when depth comparing.
	 */
	enum class DepthCompareLogic : UI8 {
		Never,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual,
		Always,
	};

	/**
	 * Color blend factor enum.
	 */
	enum class ColorBlendFactor : UI8 {
		Zero,
		One,
		SourceColor,
		OneMinusSourceColor,
		DestinationColor,
		OneMinusDestinationColor,
		SourceAlpha,
		OneMinusSourceAlpha,
		DestinationAlpha,
		OneMinusDestinationAlpha,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha,
		SourceAlphaSaturate,
		SourceOneColor,
		OneMinusSourceOneColor,
		SourceOneAlpha,
		OneMinusSourceOneAlpha,
	};

	/**
	 * Color blend operator enum.
	 */
	enum class ColorBlendOperator : UI8 {
		Add,
		Subtract,
		ReverseSubtract,
		Minimum,
		Maximum,
		Zero,
		Source,
		Destination,
		SourceOver,
		DestinationOver,
		SourceIn,
		DestinationIn,
		SouceOut,
		DestinationOut,
		SourceATOP,
		DestinationATOP,
		XOR,
		Multiply,
		Screen,
		Overlay,
		Darken,
		Lighten,
		ColorDodge,
		ColorBurn,
		HardLight,
		SoftLight,
		Difference,
		Exclusion,
		Invert,
		InvertRGB,
		LinearDodge,
		LinearBurn,
		VividLight,
		LinearLight,
		PinLight,
		HardMix,
		HSLHue,
		HSLSaturation,
		HSLColor,
		HSLLuminosity,
		Plus,
		PlusClamped,
		PlusClampedAlpha,
		PlusDarker,
		Minus,
		MinusClamped,
		Contrast,
		InvertOVG,
		Red,
		Green,
		Blue,
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
	 * Color write mask bitwise And operator.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The And performed color write mask.
	 */
	constexpr ColorWriteMask operator&(const ColorWriteMask& lhs, const ColorWriteMask& rhs) { return static_cast<ColorWriteMask>(static_cast<UI8>(lhs) & static_cast<UI8>(rhs)); }

	/**
	 * Color blend attachment structure.
	 */
	struct ColorBlendAttachment
	{
		bool mEnableBlend = false;
		ColorBlendFactor mSrcBlendFactor = ColorBlendFactor::Zero;
		ColorBlendFactor mDstBlendFactor = ColorBlendFactor::Zero;
		ColorBlendFactor mSrcAlphaBlendFactor = ColorBlendFactor::Zero;
		ColorBlendFactor mDstAlphaBlendFactor = ColorBlendFactor::Zero;
		ColorBlendOperator mBlendOperator = ColorBlendOperator::Add;
		ColorBlendOperator mAlphaBlendOperator = ColorBlendOperator::Add;
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

		PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::TriangleList;
		CullMode mCullMode = CullMode::Back;
		FrontFace mFrontFace = FrontFace::CounterClockwise;
		PolygonMode mPolygonMode = PolygonMode::Fill;
		ColorBlendLogic mColorBlendLogic = ColorBlendLogic::Clear;
		DepthCompareLogic mDepthCompareLogic = DepthCompareLogic::LessOrEqual;
		DynamicStateFlags mDynamicStateFlags = DynamicStateFlags(0);
		MultiSampleCount mRasterizationSamples = MultiSampleCount::One;

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
		virtual std::shared_ptr<ResourceMap> CreateResourceMap() const override final;

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