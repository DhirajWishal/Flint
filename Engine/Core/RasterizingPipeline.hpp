// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "RasterizingProgram.hpp"

#include <functional>

namespace Flint
{
	class Rasterizer;

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
		// In this mode, the front side of it is facing away from you.
		CounterClockwise,

		// In this mode, the front is facing towards you.
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
		R = 1 << 0,
		G = 1 << 1,
		B = 1 << 2,
		A = 1 << 3,
	};

	FLINT_DEFINE_ENUM_AND_OR(ColorWriteMask);

	/**
	 * Color blend attachment structure.
	 */
	struct ColorBlendAttachment final
	{
		bool m_EnableBlend = false;
		ColorBlendFactor m_SrcBlendFactor = ColorBlendFactor::Zero;
		ColorBlendFactor m_DstBlendFactor = ColorBlendFactor::Zero;
		ColorBlendFactor m_SrcAlphaBlendFactor = ColorBlendFactor::Zero;
		ColorBlendFactor m_DstAlphaBlendFactor = ColorBlendFactor::Zero;
		ColorBlendOperator m_BlendOperator = ColorBlendOperator::Add;
		ColorBlendOperator m_AlphaBlendOperator = ColorBlendOperator::Add;
		ColorWriteMask m_ColorWriteMask = ColorWriteMask::R | ColorWriteMask::G | ColorWriteMask::B | ColorWriteMask::A;
	};

	/**
	 * Dynamic state flags.
	 * This determines which dynamic states the pipeline consists.
	 */
	enum class DynamicStateFlags : uint8_t
	{
		Undefined,
		ViewPort = 1 << 0,
		Scissor = 1 << 1,
		LineWidth = 1 << 2,
		DepthBias = 1 << 3,
		BlendConstants = 1 << 4,
		DepthBounds = 1 << 5
	};

	FLINT_DEFINE_ENUM_AND_OR(DynamicStateFlags);

	/**
	 * Rasterizing pipeline specification structure.
	 * This structure defines the rasterizing pipeline.
	 *
	 * In the vertex shader, the first few inputs will be dedicated to the vertex descriptor's attributes. The rest will be used for instancing.
	 * Make sure that all the resources are from set = 0.
	 */
	struct RasterizingPipelineSpecification final
	{
		std::vector<ColorBlendAttachment> m_ColorBlendAttachments = { ColorBlendAttachment() };

		float m_ColorBlendConstants[4] = {};
		float m_DepthBiasFactor = 0.0f;
		float m_DepthConstantFactor = 0.0f;
		float m_DepthSlopeFactor = 0.0f;
		float m_RasterizerLineWidth = 1.0f;
		float m_MinSampleShading = 1.0f;

		uint32_t m_TessellationPatchControlPoints = 0;

		PrimitiveTopology m_PrimitiveTopology = PrimitiveTopology::TriangleList;
		CullMode m_CullMode = CullMode::Back;
		FrontFace m_FrontFace = FrontFace::CounterClockwise;
		PolygonMode m_PolygonMode = PolygonMode::Fill;
		ColorBlendLogic m_ColorBlendLogic = ColorBlendLogic::Clear;
		DepthCompareLogic m_DepthCompareLogic = DepthCompareLogic::LessOrEqual;
		DynamicStateFlags m_DynamicStateFlags = DynamicStateFlags::Undefined;

		bool m_EnablePrimitiveRestart : 1 = false;
		bool m_EnableDepthBias : 1 = false;
		bool m_EnableDepthClamp : 1 = false;
		bool m_EnableRasterizerDiscard : 1 = false;
		bool m_EnableAlphaCoverage : 1 = false;
		bool m_EnableAlphaToOne : 1 = false;
		bool m_EnableSampleShading : 1 = true;
		bool m_EnableColorBlendLogic : 1 = false;
		bool m_EnableDepthTest : 1 = true;
		bool m_EnableDepthWrite : 1 = true;
	};

	/**
	 * Rasterizing pipeline class.
	 * This class is used to perform rasterizing graphics.
	 */
	class RasterizingPipeline : public Pipeline
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device to which the pipeline is bound to.
		 * @param pRasterizer The parent rasterizer.
		 * @param pProgram The rasterizing program used in the pipeline.
		 * @param specification The pipeline specification.
		 * @param pCacheHandler The pipeline cache handler. Default is nullptr.
		 */
		explicit RasterizingPipeline(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Rasterizer>& pRasterizer, const std::shared_ptr<RasterizingProgram>& pProgram, const RasterizingPipelineSpecification& specification, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler = nullptr)
			: Pipeline(pDevice, std::move(pCacheHandler)), m_pRasterizer(pRasterizer), m_pProgram(pProgram), m_Specification(specification) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~RasterizingPipeline() = default;

		/**
		 * Get the parent rasterizer.
		 *
		 * @return The rasterizer reference.
		 */
		[[nodiscard]] const Rasterizer& getRasterizer() const { return *m_pRasterizer; }

		/**
		 * Get the parent rasterizer.
		 *
		 * @return The rasterizer reference.
		 */
		[[nodiscard]] Rasterizer& getRasterizer() { return *m_pRasterizer; }

		/**
		 * Get the pipeline specification.
		 *
		 * @return The specification reference.
		 */
		[[nodiscard]] const RasterizingPipelineSpecification& getSpecification() const { return m_Specification; }

	protected:
		std::shared_ptr<Rasterizer> m_pRasterizer = nullptr;
		std::shared_ptr<RasterizingProgram> m_pProgram = nullptr;
		RasterizingPipelineSpecification m_Specification;
	};
}