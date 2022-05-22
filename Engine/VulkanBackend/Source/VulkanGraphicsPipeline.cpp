// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace /* anonymous */
{
	//VkFormat GetFormatFromSize(ShaderAttributeDataType type)
	//{
	//	switch (type)
	//	{
	//	case Flint::ShaderAttributeDataType::VEC1:
	//		return VkFormat::VK_FORMAT_R32_UINT;
	//
	//	case Flint::ShaderAttributeDataType::VEC2:
	//		return VkFormat::VK_FORMAT_R32G32_SFLOAT;
	//
	//	case Flint::ShaderAttributeDataType::VEC3:
	//		return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
	//
	//	case Flint::ShaderAttributeDataType::VEC4:
	//		return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
	//
	//	default:
	//		throw Flint::BackendError("Invalid or unsupported shader attribute type!");
	//	}
	//
	//	return VkFormat::VK_FORMAT_UNDEFINED;
	//}

	/**
	 * Get the primitive topology.
	 *
	 * @param topology The flint primitive topology.
	 * @return The Vulkan primitive topology.
	 */
	VkPrimitiveTopology GetPrimitiveTopology(Flint::PrimitiveTopology topology)
	{
		switch (topology)
		{
		case Flint::PrimitiveTopology::PointList:					return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case Flint::PrimitiveTopology::LineList:					return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case Flint::PrimitiveTopology::LineStrip:					return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case Flint::PrimitiveTopology::TriangleList:				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case Flint::PrimitiveTopology::TriangleStrip:				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case Flint::PrimitiveTopology::TriangleFan:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		case Flint::PrimitiveTopology::LineListWithAdjacency:		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
		case Flint::PrimitiveTopology::LineStripWithAdjacency:		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
		case Flint::PrimitiveTopology::TriangleListWithAdjacency:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
		case Flint::PrimitiveTopology::TriangleStripWithAdjacency:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
		case Flint::PrimitiveTopology::PatchList:					return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		default:													throw Flint::BackendError("Invalid or Undefined primitive topology!");
		}
	}

	/**
	 * Get the cull mode.
	 *
	 * @param cull The flint cull mode.
	 * @return The Vulkan cull mode.
	 */
	VkCullModeFlags GetCullMode(Flint::CullMode cull)
	{
		switch (cull)
		{
		case Flint::CullMode::None:									return VK_CULL_MODE_NONE;
		case Flint::CullMode::Front:								return VK_CULL_MODE_FRONT_BIT;
		case Flint::CullMode::Back:									return VK_CULL_MODE_BACK_BIT;
		case Flint::CullMode::FrontAndBack:							return VK_CULL_MODE_FRONT_AND_BACK;
		default:													throw Flint::BackendError("Invalid or Undefined cull mode!");
		}
	}

	/**
	 * Get the front face.
	 *
	 * @param face The flint front face.
	 * @return The Vulkan front face.
	 */
	VkFrontFace GetFrontFace(Flint::FrontFace face)
	{
		switch (face)
		{
		case Flint::FrontFace::CounterClockwise:					return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case Flint::FrontFace::Clockwise:							return VK_FRONT_FACE_CLOCKWISE;
		default:													throw Flint::BackendError("Invalid or Undefined front face!");
		}
	}

	/**
	 * Get the polygon mode.
	 *
	 * @param mode The flint polygon mode.
	 * @return The Vulkan polygon mode.
	 */
	VkPolygonMode GetPolygonMode(Flint::PolygonMode mode)
	{
		switch (mode)
		{
		case Flint::PolygonMode::Fill:								return VK_POLYGON_MODE_FILL;
		case Flint::PolygonMode::Line:								return VK_POLYGON_MODE_LINE;
		case Flint::PolygonMode::Point:								return VK_POLYGON_MODE_POINT;
		default:													throw Flint::BackendError("Invalid or Undefined polygon mode!");
		}
	}

	/**
	 * Get the logic operator.
	 *
	 * @param logic The flint logic.
	 * @return The Vulkan logic.
	 */
	VkLogicOp GetLogicOp(Flint::ColorBlendLogic logic)
	{
		switch (logic)
		{
		case Flint::ColorBlendLogic::Clear:							return VK_LOGIC_OP_CLEAR;
		case Flint::ColorBlendLogic::And:							return VK_LOGIC_OP_AND;
		case Flint::ColorBlendLogic::AndReverse:					return VK_LOGIC_OP_AND_REVERSE;
		case Flint::ColorBlendLogic::Copy:							return VK_LOGIC_OP_COPY;
		case Flint::ColorBlendLogic::AndInverted:					return VK_LOGIC_OP_AND_INVERTED;
		case Flint::ColorBlendLogic::NoOperator:					return VK_LOGIC_OP_NO_OP;
		case Flint::ColorBlendLogic::XOR:							return VK_LOGIC_OP_XOR;
		case Flint::ColorBlendLogic::OR:							return VK_LOGIC_OP_OR;
		case Flint::ColorBlendLogic::NOR:							return VK_LOGIC_OP_NOR;
		case Flint::ColorBlendLogic::Equivalent:					return VK_LOGIC_OP_EQUIVALENT;
		case Flint::ColorBlendLogic::Invert:						return VK_LOGIC_OP_INVERT;
		case Flint::ColorBlendLogic::ReverseOR:						return VK_LOGIC_OP_OR_REVERSE;
		case Flint::ColorBlendLogic::CopyInverted:					return VK_LOGIC_OP_COPY_INVERTED;
		case Flint::ColorBlendLogic::InvertedOR:					return VK_LOGIC_OP_OR_INVERTED;
		case Flint::ColorBlendLogic::NAND:							return VK_LOGIC_OP_NAND;
		case Flint::ColorBlendLogic::Set:							return VK_LOGIC_OP_SET;
		default:													throw Flint::BackendError("Invalid or Undefined color blend logic!");
		}
	}

	/**
	 * Get the compare operator.
	 *
	 * @param logic the flint logic.
	 * @return The Vulkan logic operator.
	 */
	VkCompareOp GetCompareOp(Flint::DepthCompareLogic logic)
	{
		switch (logic)
		{
		case Flint::DepthCompareLogic::Never:						return VK_COMPARE_OP_NEVER;
		case Flint::DepthCompareLogic::Less:						return VK_COMPARE_OP_LESS;
		case Flint::DepthCompareLogic::Equal:						return VK_COMPARE_OP_EQUAL;
		case Flint::DepthCompareLogic::LessOrEqual:					return VK_COMPARE_OP_LESS_OR_EQUAL;
		case Flint::DepthCompareLogic::Greater:						return VK_COMPARE_OP_GREATER;
		case Flint::DepthCompareLogic::NotEqual:					return VK_COMPARE_OP_NOT_EQUAL;
		case Flint::DepthCompareLogic::GreaterOrEqual:				return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case Flint::DepthCompareLogic::Always:						return VK_COMPARE_OP_ALWAYS;
		default:													throw Flint::BackendError("Invalid or Undefined depth compare logic!");
		}
	}

	/**
	 * Get the dynamic states.
	 *
	 * @param flags The flint flags.
	 * @return The Vulkan flags.
	 */
	std::vector<VkDynamicState> GetDynamicStates(Flint::DynamicStateFlags flags)
	{
		std::vector<VkDynamicState> states;
		if ((flags & Flint::DynamicStateFlags::ViewPort) == Flint::DynamicStateFlags::ViewPort) states.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		if ((flags & Flint::DynamicStateFlags::Scissor) == Flint::DynamicStateFlags::Scissor) states.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
		if ((flags & Flint::DynamicStateFlags::LineWidth) == Flint::DynamicStateFlags::LineWidth) states.emplace_back(VK_DYNAMIC_STATE_LINE_WIDTH);
		if ((flags & Flint::DynamicStateFlags::DepthBias) == Flint::DynamicStateFlags::DepthBias) states.emplace_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
		if ((flags & Flint::DynamicStateFlags::BlendConstants) == Flint::DynamicStateFlags::BlendConstants) states.emplace_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
		if ((flags & Flint::DynamicStateFlags::DepthBounds) == Flint::DynamicStateFlags::DepthBounds) states.emplace_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);

		return states;
	}

	/**
	 * Get the blend factor.
	 *
	 * @param factor The flint factor.
	 * @return The Vulkan factor.
	 */
	VkBlendFactor GetBlendFactor(Flint::ColorBlendFactor factor)
	{
		switch (factor)
		{
		case Flint::ColorBlendFactor::Zero:							return VK_BLEND_FACTOR_ZERO;
		case Flint::ColorBlendFactor::One:							return VK_BLEND_FACTOR_ONE;
		case Flint::ColorBlendFactor::SourceColor:					return VK_BLEND_FACTOR_SRC_COLOR;
		case Flint::ColorBlendFactor::OneMinusSourceColor:			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case Flint::ColorBlendFactor::DestinationColor:				return VK_BLEND_FACTOR_DST_COLOR;
		case Flint::ColorBlendFactor::OneMinusDestinationColor:		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case Flint::ColorBlendFactor::SourceAlpha:					return VK_BLEND_FACTOR_SRC_ALPHA;
		case Flint::ColorBlendFactor::OneMinusSourceAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case Flint::ColorBlendFactor::DestinationAlpha:				return VK_BLEND_FACTOR_DST_ALPHA;
		case Flint::ColorBlendFactor::OneMinusDestinationAlpha:		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case Flint::ColorBlendFactor::ConstantColor:				return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case Flint::ColorBlendFactor::OneMinusConstantColor:		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		case Flint::ColorBlendFactor::ConstantAlpha:				return VK_BLEND_FACTOR_CONSTANT_ALPHA;
		case Flint::ColorBlendFactor::OneMinusConstantAlpha:		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
		case Flint::ColorBlendFactor::SourceAlphaSaturate:			return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		case Flint::ColorBlendFactor::SourceOneColor:				return VK_BLEND_FACTOR_SRC1_COLOR;
		case Flint::ColorBlendFactor::OneMinusSourceOneColor:		return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
		case Flint::ColorBlendFactor::SourceOneAlpha:				return VK_BLEND_FACTOR_SRC1_ALPHA;
		case Flint::ColorBlendFactor::OneMinusSourceOneAlpha:		return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
		default:													throw Flint::BackendError("Invalid color blend factor!");
		}
	}

	/**
	 * Get the blend operator.
	 *
	 * @param oprtr The flint operator.
	 * @return The Vulkan blend operator.
	 */
	VkBlendOp GetBlendOp(Flint::ColorBlendOperator oprtr)
	{
		switch (oprtr)
		{
		case Flint::ColorBlendOperator::Add:						return VK_BLEND_OP_ADD;
		case Flint::ColorBlendOperator::Subtract:					return VK_BLEND_OP_SUBTRACT;
		case Flint::ColorBlendOperator::ReverseSubtract:			return VK_BLEND_OP_REVERSE_SUBTRACT;
		case Flint::ColorBlendOperator::Minimum:					return VK_BLEND_OP_MIN;
		case Flint::ColorBlendOperator::Maximum:					return VK_BLEND_OP_MAX;
		case Flint::ColorBlendOperator::Zero:						return VK_BLEND_OP_ZERO_EXT;
		case Flint::ColorBlendOperator::Source:						return VK_BLEND_OP_SRC_EXT;
		case Flint::ColorBlendOperator::Destination:				return VK_BLEND_OP_DST_EXT;
		case Flint::ColorBlendOperator::SourceOver:					return VK_BLEND_OP_SRC_OVER_EXT;
		case Flint::ColorBlendOperator::DestinationOver:			return VK_BLEND_OP_DST_OVER_EXT;
		case Flint::ColorBlendOperator::SourceIn:					return VK_BLEND_OP_SRC_IN_EXT;
		case Flint::ColorBlendOperator::DestinationIn:				return VK_BLEND_OP_DST_IN_EXT;
		case Flint::ColorBlendOperator::SouceOut:					return VK_BLEND_OP_SRC_OUT_EXT;
		case Flint::ColorBlendOperator::DestinationOut:				return VK_BLEND_OP_DST_OUT_EXT;
		case Flint::ColorBlendOperator::SourceATOP:					return VK_BLEND_OP_SRC_ATOP_EXT;
		case Flint::ColorBlendOperator::DestinationATOP:			return VK_BLEND_OP_DST_ATOP_EXT;
		case Flint::ColorBlendOperator::XOR:						return VK_BLEND_OP_XOR_EXT;
		case Flint::ColorBlendOperator::Multiply:					return VK_BLEND_OP_MULTIPLY_EXT;
		case Flint::ColorBlendOperator::Screen:						return VK_BLEND_OP_SCREEN_EXT;
		case Flint::ColorBlendOperator::Overlay:					return VK_BLEND_OP_OVERLAY_EXT;
		case Flint::ColorBlendOperator::Darken:						return VK_BLEND_OP_DARKEN_EXT;
		case Flint::ColorBlendOperator::Lighten:					return VK_BLEND_OP_LIGHTEN_EXT;
		case Flint::ColorBlendOperator::ColorDodge:					return VK_BLEND_OP_COLORDODGE_EXT;
		case Flint::ColorBlendOperator::ColorBurn:					return VK_BLEND_OP_COLORBURN_EXT;
		case Flint::ColorBlendOperator::HardLight:					return VK_BLEND_OP_HARDLIGHT_EXT;
		case Flint::ColorBlendOperator::SoftLight:					return VK_BLEND_OP_SOFTLIGHT_EXT;
		case Flint::ColorBlendOperator::Difference:					return VK_BLEND_OP_DIFFERENCE_EXT;
		case Flint::ColorBlendOperator::Exclusion:					return VK_BLEND_OP_EXCLUSION_EXT;
		case Flint::ColorBlendOperator::Invert:						return VK_BLEND_OP_INVERT_EXT;
		case Flint::ColorBlendOperator::InvertRGB:					return VK_BLEND_OP_INVERT_RGB_EXT;
		case Flint::ColorBlendOperator::LinearDodge:				return VK_BLEND_OP_LINEARDODGE_EXT;
		case Flint::ColorBlendOperator::LinearBurn:					return VK_BLEND_OP_LINEARBURN_EXT;
		case Flint::ColorBlendOperator::VividLight:					return VK_BLEND_OP_VIVIDLIGHT_EXT;
		case Flint::ColorBlendOperator::LinearLight:				return VK_BLEND_OP_LINEARLIGHT_EXT;
		case Flint::ColorBlendOperator::PinLight:					return VK_BLEND_OP_PINLIGHT_EXT;
		case Flint::ColorBlendOperator::HardMix:					return VK_BLEND_OP_HARDMIX_EXT;
		case Flint::ColorBlendOperator::HSLHue:						return VK_BLEND_OP_HSL_HUE_EXT;
		case Flint::ColorBlendOperator::HSLSaturation:				return VK_BLEND_OP_HSL_SATURATION_EXT;
		case Flint::ColorBlendOperator::HSLColor:					return VK_BLEND_OP_HSL_COLOR_EXT;
		case Flint::ColorBlendOperator::HSLLuminosity:				return VK_BLEND_OP_HSL_LUMINOSITY_EXT;
		case Flint::ColorBlendOperator::Plus:						return VK_BLEND_OP_PLUS_EXT;
		case Flint::ColorBlendOperator::PlusClamped:				return VK_BLEND_OP_PLUS_CLAMPED_EXT;
		case Flint::ColorBlendOperator::PlusClampedAlpha:			return VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT;
		case Flint::ColorBlendOperator::PlusDarker:					return VK_BLEND_OP_PLUS_DARKER_EXT;
		case Flint::ColorBlendOperator::Minus:						return VK_BLEND_OP_MINUS_EXT;
		case Flint::ColorBlendOperator::MinusClamped:				return VK_BLEND_OP_MINUS_CLAMPED_EXT;
		case Flint::ColorBlendOperator::Contrast:					return VK_BLEND_OP_CONTRAST_EXT;
		case Flint::ColorBlendOperator::InvertOVG:					return VK_BLEND_OP_INVERT_OVG_EXT;
		case Flint::ColorBlendOperator::Red:						return VK_BLEND_OP_RED_EXT;
		case Flint::ColorBlendOperator::Green:						return VK_BLEND_OP_GREEN_EXT;
		case Flint::ColorBlendOperator::Blue:						return VK_BLEND_OP_BLUE_EXT;
		default:													throw Flint::BackendError("Invalid color blend operator!");
		}
	}

	/**
	 * Get the color component flags.
	 *
	 * @param mask The color write mask.
	 * @return The VUlkan mask.
	 */
	VkColorComponentFlags GetComponentFlags(Flint::ColorWriteMask mask)
	{
		VkColorComponentFlags flags = 0;
		if ((mask & Flint::ColorWriteMask::R) == Flint::ColorWriteMask::R) flags |= VK_COLOR_COMPONENT_R_BIT;
		if ((mask & Flint::ColorWriteMask::G) == Flint::ColorWriteMask::G) flags |= VK_COLOR_COMPONENT_G_BIT;
		if ((mask & Flint::ColorWriteMask::B) == Flint::ColorWriteMask::B) flags |= VK_COLOR_COMPONENT_B_BIT;
		if ((mask & Flint::ColorWriteMask::A) == Flint::ColorWriteMask::A) flags |= VK_COLOR_COMPONENT_A_BIT;

		return flags;
	}
}

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanEngine& engine, std::string&& name, const PipelineIdentifier& identifier, RasterizingPipelineSpecification&& specification)
			: VulkanPipeline(engine, std::move(name)), m_Specification(specification)
		{
		}

		VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
		{
		}

		void VulkanGraphicsPipeline::createDescriptorSetLayout()
		{

		}

		void VulkanGraphicsPipeline::destroyDescriptorSetLayout()
		{

		}

		void VulkanGraphicsPipeline::createPipelineLayout()
		{

		}
	}
}