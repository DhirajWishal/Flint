// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"
#include "ShaderCode.hpp"

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
	struct ColorBlendAttachment final
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

	/**
	 * Rasterizing pipeline specification structure.
	 * This structure defines the rasterizing pipeline.
	 */
	struct RasterizingPipelineSpecification final
	{
		ShaderCode m_VertexShader = {};
		ShaderCode m_TessellationControlShader = {};
		ShaderCode m_TessellationEvaluationShader = {};
		ShaderCode m_GeometryShader = {};
		ShaderCode m_FragmentShader = {};

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
	 * Rasterizer class.
	 * This class performs rasterization to the bound entities.
	 */
	class Rasterizer : public RenderTarget
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine reference.
		 * @param width The width of the render target.
		 * @param height The height of the render target.
		 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
		 * @param attachmentDescriptions The attachment descriptions.
		 * @param multisample The multisample count. Default is One.
		 * @param exclusiveBuffering Whether or not to use one buffer/ attachment per frame. Default is false.
		 */
		explicit Rasterizer(Engine& engine, uint32_t width, uint32_t height, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One, bool exclusiveBuffering = false)
			: RenderTarget(engine, width, height, frameCount, std::move(attachmentDescriptions), multisample, exclusiveBuffering) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~Rasterizer() = default;

		/**
		 * Register a new rasterizing pipeline.
		 *
		 * @param identifier The identifier to which the pipeline is associated with.
		 * @param name The name of the pipeline.
		 * @param specification The pipeline specification.
		 */
		virtual void registerPipeline(const PipelineIdentifier& identifier, std::string&& name, RasterizingPipelineSpecification&& specification) = 0;
	};
}