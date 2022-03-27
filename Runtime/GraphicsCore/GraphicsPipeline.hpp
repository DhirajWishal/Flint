// Copyright 2021-2022 Dhiraj Wishal
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
	enum class PrimitiveTopology : uint8_t
	{
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
	enum class CullMode : uint8_t
	{
		None,
		Front,
		Back,
		FrontAndBack
	};

	/**
	 * Pipeline front face.
	 * This defines how to find the front size of a triangle.
	 */
	enum class FrontFace : uint8_t
	{
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
	enum class PolygonMode : uint8_t
	{
		Fill,
		Line,
		Point,
	};

	/**
	 * Pipeline color blend logic.
	 * This defined what logic to apply when color blending.
	 */
	enum class ColorBlendLogic : uint8_t
	{
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
	enum class DepthCompareLogic : uint8_t
	{
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
	enum class ColorBlendFactor : uint8_t
	{
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
	enum class ColorBlendOperator : uint8_t
	{
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
	enum class ColorWriteMask : uint8_t
	{
		R = BitShiftLeft(0),
		G = BitShiftLeft(1),
		B = BitShiftLeft(2),
		A = BitShiftLeft(3),
	};

	/**
	 * Color write mask bitwise OR operator.
	 *
	 * @param lhs The left hand side argument.
	 * @param rhs The right hand side argument.
	 * @return The OR performed color write mask.
	 */
	constexpr ColorWriteMask operator|(const ColorWriteMask& lhs, const ColorWriteMask& rhs) { return static_cast<ColorWriteMask>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)); }

	/**
	 * Color write mask bitwise And operator.
	 *
	 * @param lhs The left hand side argument.
	 * @param rhs The right hand side argument.
	 * @return The And performed color write mask.
	 */
	constexpr ColorWriteMask operator&(const ColorWriteMask& lhs, const ColorWriteMask& rhs) { return static_cast<ColorWriteMask>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)); }

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

		TShaderAttributeMap mVertexInputAttributeMap = {};
		std::vector<ColorBlendAttachment> mColorBlendAttachments = { ColorBlendAttachment() };

		float mColorBlendConstants[4] = {
			CreateColor256(0), CreateColor256(0),
			CreateColor256(0), CreateColor256(0)
		};
		float mDepthBiasFactor = 0.0f;
		float mDepthConstantFactor = 0.0f;
		float mDepthSlopeFactor = 0.0f;
		float mRasterizerLineWidth = 1.0f;
		float mMinSampleShading = 1.0f;

		uint32_t mTessellationPatchControlPoints = 0;

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
	template<class DeviceT, class RenderTargetT, class ResourcePackageT, class ShaderT, class ScreenBoundRenderTargetT, class OffScreenRenderTargetT>
	class GraphicsPipeline : public Pipeline<DeviceT, RenderTargetT, ResourcePackageT>
	{
	public:
		/**
		 * Construct the pipeline using a screen bound render target.
		 *
		 * @param pDevice The device pointer.
		 * @param pipelineName The name of the pipeline.
		 * @param pScreenBoundRenderTarget The screen bound render target pointer.
		 * @param pVertexShader The vertex shader pointer.
		 * @param pTessellationControlShader The tessellation control shader (optional).
		 * @param pTessellationEvaluationShader The tessellation evaluation shader (optional).
		 * @param pGeometryShader The geometry shader (optional).
		 * @param pFragmentShader The fragment shader (optional).
		 * @param specification The pipeline specification.
		 */
		GraphicsPipeline(DeviceT* pDevice, const std::string& pipelineName, const ScreenBoundRenderTargetT* pScreenBoundRenderTarget, std::unique_ptr<ShaderT>&& pVertexShader, std::unique_ptr<ShaderT>&& pTessellationControlShader, std::unique_ptr<ShaderT>&& pTessellationEvaluationShader, std::unique_ptr<ShaderT>&& pGeometryShader, std::unique_ptr<ShaderT>&& pFragmentShader, const GraphicsPipelineSpecification& specification)
			: Pipeline(pDevice, pipelineName)
			, pRenderTarget(pScreenBoundRenderTarget)
			, pVertexShader(std::move(pVertexShader))
			, pTessellationControlShader(std::move(pTessellationControlShader))
			, pTessellationEvaluationShader(std::move(pTessellationEvaluationShader))
			, pGeometryShader(std::move(pGeometryShader))
			, pFragmentShader(std::move(pFragmentShader))
			, mSpecification(specification)
		{
			if (!pScreenBoundRenderTarget)
				throw std::invalid_argument("Render target pointer should not be null!");

			if (pVertexShader == nullptr)
				throw std::invalid_argument("Vertex shader pointer should not be null!");

			if (pFragmentShader == nullptr)
				throw std::invalid_argument("Fragment shader pointer should not be null!");
		}

		/**
		 * Construct the pipeline using an off screen render target.
		 *
		 * @param pDevice The device pointer.
		 * @param pipelineName The name of the pipeline.
		 * @param pOffScreenRenderTarget The off screen render target pointer.
		 * @param pVertexShader The vertex shader pointer.
		 * @param pTessellationControlShader The tessellation control shader (optional).
		 * @param pTessellationEvaluationShader The tessellation evaluation shader (optional).
		 * @param pGeometryShader The geometry shader (optional).
		 * @param pFragmentShader The fragment shader (optional).
		 * @param specification The pipeline specification.
		 */
		GraphicsPipeline(DeviceT* pDevice, const std::string& pipelineName, const OffScreenRenderTargetT* pOffScreenRenderTarget, std::unique_ptr<ShaderT>&& pVertexShader, std::unique_ptr<ShaderT>&& pTessellationControlShader, std::unique_ptr<ShaderT>&& pTessellationEvaluationShader, std::unique_ptr<ShaderT>&& pGeometryShader, std::unique_ptr<ShaderT>&& pFragmentShader, const GraphicsPipelineSpecification& specification)
			: Pipeline(pDevice, pipelineName)
			, pRenderTarget(pOffScreenRenderTarget)
			, pVertexShader(std::move(pVertexShader))
			, pTessellationControlShader(std::move(pTessellationControlShader))
			, pTessellationEvaluationShader(std::move(pTessellationEvaluationShader))
			, pGeometryShader(std::move(pGeometryShader))
			, pFragmentShader(std::move(pFragmentShader))
			, mSpecification(specification)
		{
			if (!pOffScreenRenderTarget)
				throw std::invalid_argument("Render target pointer should not be null!");

			if (pVertexShader == nullptr)
				throw std::invalid_argument("Vertex shader pointer should not be null!");
		}

		/**
		 * Recreate the graphics pipeline.
		 * This is needed once the render target is resized.
		 *
		 * @param pScreenBoundRenderTarget The screen bound render target pointer.
		 */
		virtual void Recreate(ScreenBoundRenderTargetT* pScreenBoundRenderTarget) = 0;

		/**
		 * Get the current graphics specification of the pipeline.
		 *
		 * @return The graphics specification.
		 */
		const GraphicsPipelineSpecification GetSpecification() const { return mSpecification; }

	public:
		/**
		 * Get the vertex shader pointer.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetVertexShader() const { return pVertexShader.get(); }

		/**
		 * Get the fragment shader pointer.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetFragmentShader() const { return pFragmentShader.get(); }

		/**
		 * Get the tessellation control shader pointer.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetTessellationControlShader() const { return pTessellationControlShader.get(); }

		/**
		 * Get the tessellation evaluation shader pointer.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetTessellationEvaluationShader() const { return pTessellationEvaluationShader.get(); }

		/**
		 * Get the geometry shader pointer.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetGeometryShader() const { return pGeometryShader.get(); }

	protected:
		GraphicsPipelineSpecification mSpecification = {};

		const RenderTarget* pRenderTarget = nullptr;

		std::unique_ptr<ShaderT> pVertexShader = nullptr;
		std::unique_ptr<ShaderT> pFragmentShader = nullptr;
		std::unique_ptr<ShaderT> pTessellationControlShader = nullptr;
		std::unique_ptr<ShaderT> pTessellationEvaluationShader = nullptr;
		std::unique_ptr<ShaderT> pGeometryShader = nullptr;
	};
}