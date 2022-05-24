// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderCode.hpp"
#include "Types.hpp"

#include <functional>

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
	 * Vertex attribute type enum.
	 */
	enum class VertexAttributeType : uint8_t
	{
		Float,		// 32 bit float type.

		Vec2_8,		// Two component, 8 bit vector.
		Vec2_16,	// Two component, 16 bit vector.
		Vec2_32,	// Two component, 32 bit vector.
		Vec2_64,	// Two component, 64 bit vector.

		Vec3_8,		// Three component, 8 bit vector.
		Vec3_16,	// Three component, 16 bit vector.
		Vec3_32,	// Three component, 32 bit vector.
		Vec3_64,	// Three component, 64 bit vector.

		Vec4_8,		// Four component, 8 bit vector.
		Vec4_16,	// Four component, 16 bit vector.
		Vec4_32,	// Four component, 32 bit vector.
		Vec4_64,	// Four component, 64 bit vector.
	};

	/**
	 * Input binding type enum.
	 */
	enum class InputBindingType : uint8_t
	{
		VertexData,
		InstanceData
	};

	/**
	 * Input binding structure.
	 */
	struct InputBinding final
	{
		/**
		 * Explicit constructor.
		 *
		 * @param type The type of the data.
		 */
		explicit InputBinding(InputBindingType type) : m_Type(type) {}

		/**
		 * Add a new attribute.
		 * Make sure that order in which the attributes are submitted, are the same order in which they are visible in the shader.
		 *
		 * @param location The location of the attribute.
		 * @param type The type of the attributes.
		 */
		void add(uint32_t location, VertexAttributeType type) { m_Attributes.emplace_back(location, type); }

	public:
		std::vector<std::pair<uint32_t, VertexAttributeType>> m_Attributes;
		InputBindingType m_Type = InputBindingType::VertexData;
	};

	/**
	 * Rasterizing pipeline specification structure.
	 * This structure defines the rasterizing pipeline.
	 *
	 * In the vertex shader, the first few inputs will be dedicated to the vertex descriptor's attributes. The rest will be used for instancing.
	 * Make sure that all the resources are from set = 0.
	 */
	struct RasterizingPipelineSpecification final
	{		
		std::filesystem::path m_CacheFile;	// This file is used to store the pipeline cache. Make sure that a file is specified else the backend will throw an exception.

		ShaderCode m_VertexShader = {};	// Make sure that the input bindings match with the Vertex shader's inputs.
		ShaderCode m_TessellationControlShader = {};
		ShaderCode m_TessellationEvaluationShader = {};
		ShaderCode m_GeometryShader = {};
		ShaderCode m_FragmentShader = {};

		/**
		 * Input bindings are used to bind different vertex buffers to the vertex shader when drawing.
		 * By default, the first binding is for the vertex buffer. Second is for the instance buffer. The rest is user defined.
		 *
		 * Note: Make sure that these bindings match with the Vertex shader's bindings.
		 *
		 * Example:
		 * auto& vertexBinding = m_InputBindings.emplace_back(InputBindingType::VertexData);
		 * vertexBinding.add(0, VertexAttributeType::Vec3_32);	// layout(location = 0) in vec3 inPosition;
		 * vertexBinding.add(1, VertexAttributeType::Vec2_32);	// layout(location = 1) in vec2 inTexture;
		 *
		 * auto& instanceBinding = m_InputBindings.emplace_back(InputBindingType::InstanceData);
		 * instanceBinding.add(2, VertexAttributeType::Vec3_32);	// layout(location = 2) in vec3 instancePosition;
		 * instanceBinding.add(3, VertexAttributeType::Vec3_32);	// layout(location = 3) in vec3 instanceRotation;
		 * instanceBinding.add(4, VertexAttributeType::Vec3_32);	// layout(location = 4) in vec3 instanceScale;
		 * instanceBinding.add(5, VertexAttributeType::Float);		// layout(location = 5) in int instanceID;
		 */
		std::vector<InputBinding> m_InputBindings = {};

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
	 * Mesh rasterizer class.
	 * This class contains the pipeline specification and the data payload that is used when performing raster graphics on the mesh.
	 */
	class MeshRasterizer final
	{
	public:
		/**
		 * Default constructor.
		 */
		MeshRasterizer() = default;

		/**
		 * Get the rasterizing pipeline specification.
		 *
		 * @return The specification reference.
		 */
		[[nodiscard]] RasterizingPipelineSpecification& getSpecification() { return m_Specification; }

		/**
		 * Get the rasterizing pipeline specification.
		 *
		 * @return The specification reference.
		 */
		[[nodiscard]] const RasterizingPipelineSpecification& getSpecification() const { return m_Specification; }

		/**
		 * Get the resource binding table.
		 *
		 * @return The binding table reference.
		 */
		[[nodiscard]] ResourceBindingTable& getBindingTable() { return m_BindingTable; }

		/**
		 * Get the resource binding table.
		 *
		 * @return The binding table reference.
		 */
		[[nodiscard]] const ResourceBindingTable& getBindingTable() const { return m_BindingTable; }

	private:
		RasterizingPipelineSpecification m_Specification = {};
		ResourceBindingTable m_BindingTable = {};
	};
}