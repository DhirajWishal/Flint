// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.VulkanBackend.VulkanCommandBuffer;
module: private;

#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanShader.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanResourcePackager.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Helpers
		{
			VkFormat GetFormatFromSize(ShaderAttributeDataType type)
			{
				switch (type)
				{
				case Flint::ShaderAttributeDataType::VEC1:
					return VkFormat::VK_FORMAT_R32_UINT;

				case Flint::ShaderAttributeDataType::VEC2:
					return VkFormat::VK_FORMAT_R32G32_SFLOAT;

				case Flint::ShaderAttributeDataType::VEC3:
					return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;

				case Flint::ShaderAttributeDataType::VEC4:
					return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;

				default:
					throw backend_error("Invalid or unsupported shader attribute type!");
				}

				return VkFormat::VK_FORMAT_UNDEFINED;
			}

			VkPrimitiveTopology GetPrimitiveTopology(PrimitiveTopology topology)
			{
				switch (topology)
				{
				case Flint::PrimitiveTopology::PointList:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

				case Flint::PrimitiveTopology::LineList:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

				case Flint::PrimitiveTopology::LineStrip:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

				case Flint::PrimitiveTopology::TriangleList:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

				case Flint::PrimitiveTopology::TriangleStrip:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

				case Flint::PrimitiveTopology::TriangleFan:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

				case Flint::PrimitiveTopology::LineListWithAdjacency:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::LineStripWithAdjacency:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::TriangleListWithAdjacency:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::TriangleStripWithAdjacency:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::PatchList:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

				default:
					throw backend_error("Invalid or Undefined primitive topology!");
				}

				return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			}

			VkCullModeFlags GetCullMode(CullMode cull)
			{
				switch (cull)
				{
				case Flint::CullMode::None:
					return VkCullModeFlagBits::VK_CULL_MODE_NONE;

				case Flint::CullMode::Front:
					return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;

				case Flint::CullMode::Back:
					return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;

				case Flint::CullMode::FrontAndBack:
					return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;

				default:
					throw backend_error("Invalid or Undefined cull mode!");
				}

				return VkCullModeFlagBits::VK_CULL_MODE_NONE;
			}

			VkFrontFace GetFrontFace(FrontFace face)
			{
				switch (face)
				{
				case Flint::FrontFace::CounterClockwise:
					return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;

				case Flint::FrontFace::Clockwise:
					return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;

				default:
					throw backend_error("Invalid or Undefined front face!");
				}

				return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
			}

			VkPolygonMode GetPolygonMode(PolygonMode mode)
			{
				switch (mode)
				{
				case Flint::PolygonMode::Fill:
					return VkPolygonMode::VK_POLYGON_MODE_FILL;

				case Flint::PolygonMode::Line:
					return VkPolygonMode::VK_POLYGON_MODE_LINE;

				case Flint::PolygonMode::Point:
					return VkPolygonMode::VK_POLYGON_MODE_POINT;

				default:
					throw backend_error("Invalid or Undefined polygon mode!");
				}

				return VkPolygonMode::VK_POLYGON_MODE_FILL;
			}

			VkLogicOp GetLogicOp(ColorBlendLogic logic)
			{
				switch (logic)
				{
				case Flint::ColorBlendLogic::Clear:
					return VkLogicOp::VK_LOGIC_OP_CLEAR;

				case Flint::ColorBlendLogic::And:
					return VkLogicOp::VK_LOGIC_OP_AND;

				case Flint::ColorBlendLogic::AndReverse:
					return VkLogicOp::VK_LOGIC_OP_AND_REVERSE;

				case Flint::ColorBlendLogic::Copy:
					return VkLogicOp::VK_LOGIC_OP_COPY;

				case Flint::ColorBlendLogic::AndInverted:
					return VkLogicOp::VK_LOGIC_OP_AND_INVERTED;

				case Flint::ColorBlendLogic::NoOperator:
					return VkLogicOp::VK_LOGIC_OP_NO_OP;

				case Flint::ColorBlendLogic::XOR:
					return VkLogicOp::VK_LOGIC_OP_XOR;

				case Flint::ColorBlendLogic::OR:
					return VkLogicOp::VK_LOGIC_OP_OR;

				case Flint::ColorBlendLogic::NOR:
					return VkLogicOp::VK_LOGIC_OP_NOR;

				case Flint::ColorBlendLogic::Equivalent:
					return VkLogicOp::VK_LOGIC_OP_EQUIVALENT;

				case Flint::ColorBlendLogic::Invert:
					return VkLogicOp::VK_LOGIC_OP_INVERT;

				case Flint::ColorBlendLogic::ReverseOR:
					return VkLogicOp::VK_LOGIC_OP_OR_REVERSE;

				case Flint::ColorBlendLogic::CopyInverted:
					return VkLogicOp::VK_LOGIC_OP_COPY_INVERTED;

				case Flint::ColorBlendLogic::InvertedOR:
					return VkLogicOp::VK_LOGIC_OP_OR_INVERTED;

				case Flint::ColorBlendLogic::NAND:
					return VkLogicOp::VK_LOGIC_OP_NAND;

				case Flint::ColorBlendLogic::Set:
					return VkLogicOp::VK_LOGIC_OP_SET;

				default:
					throw backend_error("Invalid or Undefined color blend logic!");
				}

				return VkLogicOp::VK_LOGIC_OP_CLEAR;
			}

			VkCompareOp GetCompareOp(DepthCompareLogic logic)
			{
				switch (logic)
				{
				case Flint::DepthCompareLogic::Never:
					return VkCompareOp::VK_COMPARE_OP_NEVER;

				case Flint::DepthCompareLogic::Less:
					return VkCompareOp::VK_COMPARE_OP_LESS;

				case Flint::DepthCompareLogic::Equal:
					return VkCompareOp::VK_COMPARE_OP_EQUAL;

				case Flint::DepthCompareLogic::LessOrEqual:
					return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;

				case Flint::DepthCompareLogic::Greater:
					return VkCompareOp::VK_COMPARE_OP_GREATER;

				case Flint::DepthCompareLogic::NotEqual:
					return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;

				case Flint::DepthCompareLogic::GreaterOrEqual:
					return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;

				case Flint::DepthCompareLogic::Always:
					return VkCompareOp::VK_COMPARE_OP_ALWAYS;

				default:
					throw backend_error("Invalid or Undefined depth compare logic!");
				}

				return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
			}

			std::vector<VkDynamicState> GetDynamicStates(DynamicStateFlags flags)
			{
				std::vector<VkDynamicState> states;
				if (flags & DynamicStateFlags::ViewPort)
					states.emplace_back(VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT);

				if (flags & DynamicStateFlags::Scissor)
					states.emplace_back(VkDynamicState::VK_DYNAMIC_STATE_SCISSOR);

				if (flags & DynamicStateFlags::LineWidth)
					states.emplace_back(VkDynamicState::VK_DYNAMIC_STATE_LINE_WIDTH);

				if (flags & DynamicStateFlags::DepthBias)
					states.emplace_back(VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BIAS);

				if (flags & DynamicStateFlags::BlendConstants)
					states.emplace_back(VkDynamicState::VK_DYNAMIC_STATE_BLEND_CONSTANTS);

				if (flags & DynamicStateFlags::DepthBounds)
					states.emplace_back(VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BOUNDS);

				return states;
			}

			VkBlendFactor GetBlendFactor(ColorBlendFactor factor)
			{
				switch (factor)
				{
				case Flint::ColorBlendFactor::Zero:
					return VkBlendFactor::VK_BLEND_FACTOR_ZERO;

				case Flint::ColorBlendFactor::One:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE;

				case Flint::ColorBlendFactor::SourceColor:
					return VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR;

				case Flint::ColorBlendFactor::OneMinusSourceColor:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

				case Flint::ColorBlendFactor::DestinationColor:
					return VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR;

				case Flint::ColorBlendFactor::OneMinusDestinationColor:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

				case Flint::ColorBlendFactor::SourceAlpha:
					return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;

				case Flint::ColorBlendFactor::OneMinusSourceAlpha:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

				case Flint::ColorBlendFactor::DestinationAlpha:
					return VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA;

				case Flint::ColorBlendFactor::OneMinusDestinationAlpha:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

				case Flint::ColorBlendFactor::ConstantColor:
					return VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_COLOR;

				case Flint::ColorBlendFactor::OneMinusConstantColor:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

				case Flint::ColorBlendFactor::ConstantAlpha:
					return VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_ALPHA;

				case Flint::ColorBlendFactor::OneMinusConstantAlpha:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;

				case Flint::ColorBlendFactor::SourceAlphaSaturate:
					return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;

				case Flint::ColorBlendFactor::SourceOneColor:
					return VkBlendFactor::VK_BLEND_FACTOR_SRC1_COLOR;

				case Flint::ColorBlendFactor::OneMinusSourceOneColor:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;

				case Flint::ColorBlendFactor::SourceOneAlpha:
					return VkBlendFactor::VK_BLEND_FACTOR_SRC1_ALPHA;

				case Flint::ColorBlendFactor::OneMinusSourceOneAlpha:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;

				default:
					throw backend_error("Invalid color blend factor!");
				}

				return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
			}

			VkBlendOp GetBlendOp(ColorBlendOperator oprtr)
			{
				switch (oprtr)
				{
				case Flint::ColorBlendOperator::Add:
					return VkBlendOp::VK_BLEND_OP_ADD;

				case Flint::ColorBlendOperator::Subtract:
					return VkBlendOp::VK_BLEND_OP_SUBTRACT;

				case Flint::ColorBlendOperator::ReverseSubtract:
					return VkBlendOp::VK_BLEND_OP_REVERSE_SUBTRACT;

				case Flint::ColorBlendOperator::Minimum:
					return VkBlendOp::VK_BLEND_OP_MIN;

				case Flint::ColorBlendOperator::Maximum:
					return VkBlendOp::VK_BLEND_OP_MAX;

				case Flint::ColorBlendOperator::Zero:
					return VkBlendOp::VK_BLEND_OP_ZERO_EXT;

				case Flint::ColorBlendOperator::Source:
					return VkBlendOp::VK_BLEND_OP_SRC_EXT;

				case Flint::ColorBlendOperator::Destination:
					return VkBlendOp::VK_BLEND_OP_DST_EXT;

				case Flint::ColorBlendOperator::SourceOver:
					return VkBlendOp::VK_BLEND_OP_SRC_OVER_EXT;

				case Flint::ColorBlendOperator::DestinationOver:
					return VkBlendOp::VK_BLEND_OP_DST_OVER_EXT;

				case Flint::ColorBlendOperator::SourceIn:
					return VkBlendOp::VK_BLEND_OP_SRC_IN_EXT;

				case Flint::ColorBlendOperator::DestinationIn:
					return VkBlendOp::VK_BLEND_OP_DST_IN_EXT;

				case Flint::ColorBlendOperator::SouceOut:
					return VkBlendOp::VK_BLEND_OP_SRC_OUT_EXT;

				case Flint::ColorBlendOperator::DestinationOut:
					return VkBlendOp::VK_BLEND_OP_DST_OUT_EXT;

				case Flint::ColorBlendOperator::SourceATOP:
					return VkBlendOp::VK_BLEND_OP_SRC_ATOP_EXT;

				case Flint::ColorBlendOperator::DestinationATOP:
					return VkBlendOp::VK_BLEND_OP_DST_ATOP_EXT;

				case Flint::ColorBlendOperator::XOR:
					return VkBlendOp::VK_BLEND_OP_XOR_EXT;

				case Flint::ColorBlendOperator::Multiply:
					return VkBlendOp::VK_BLEND_OP_MULTIPLY_EXT;

				case Flint::ColorBlendOperator::Screen:
					return VkBlendOp::VK_BLEND_OP_SCREEN_EXT;

				case Flint::ColorBlendOperator::Overlay:
					return VkBlendOp::VK_BLEND_OP_OVERLAY_EXT;

				case Flint::ColorBlendOperator::Darken:
					return VkBlendOp::VK_BLEND_OP_DARKEN_EXT;

				case Flint::ColorBlendOperator::Lighten:
					return VkBlendOp::VK_BLEND_OP_LIGHTEN_EXT;

				case Flint::ColorBlendOperator::ColorDodge:
					return VkBlendOp::VK_BLEND_OP_COLORDODGE_EXT;

				case Flint::ColorBlendOperator::ColorBurn:
					return VkBlendOp::VK_BLEND_OP_COLORBURN_EXT;

				case Flint::ColorBlendOperator::HardLight:
					return VkBlendOp::VK_BLEND_OP_HARDLIGHT_EXT;

				case Flint::ColorBlendOperator::SoftLight:
					return VkBlendOp::VK_BLEND_OP_SOFTLIGHT_EXT;

				case Flint::ColorBlendOperator::Difference:
					return VkBlendOp::VK_BLEND_OP_DIFFERENCE_EXT;

				case Flint::ColorBlendOperator::Exclusion:
					return VkBlendOp::VK_BLEND_OP_EXCLUSION_EXT;

				case Flint::ColorBlendOperator::Invert:
					return VkBlendOp::VK_BLEND_OP_INVERT_EXT;

				case Flint::ColorBlendOperator::InvertRGB:
					return VkBlendOp::VK_BLEND_OP_INVERT_RGB_EXT;

				case Flint::ColorBlendOperator::LinearDodge:
					return VkBlendOp::VK_BLEND_OP_LINEARDODGE_EXT;

				case Flint::ColorBlendOperator::LinearBurn:
					return VkBlendOp::VK_BLEND_OP_LINEARBURN_EXT;

				case Flint::ColorBlendOperator::VividLight:
					return VkBlendOp::VK_BLEND_OP_VIVIDLIGHT_EXT;

				case Flint::ColorBlendOperator::LinearLight:
					return VkBlendOp::VK_BLEND_OP_LINEARLIGHT_EXT;

				case Flint::ColorBlendOperator::PinLight:
					return VkBlendOp::VK_BLEND_OP_PINLIGHT_EXT;

				case Flint::ColorBlendOperator::HardMix:
					return VkBlendOp::VK_BLEND_OP_HARDMIX_EXT;

				case Flint::ColorBlendOperator::HSLHue:
					return VkBlendOp::VK_BLEND_OP_HSL_HUE_EXT;

				case Flint::ColorBlendOperator::HSLSaturation:
					return VkBlendOp::VK_BLEND_OP_HSL_SATURATION_EXT;

				case Flint::ColorBlendOperator::HSLColor:
					return VkBlendOp::VK_BLEND_OP_HSL_COLOR_EXT;

				case Flint::ColorBlendOperator::HSLLuminosity:
					return VkBlendOp::VK_BLEND_OP_HSL_LUMINOSITY_EXT;

				case Flint::ColorBlendOperator::Plus:
					return VkBlendOp::VK_BLEND_OP_PLUS_EXT;

				case Flint::ColorBlendOperator::PlusClamped:
					return VkBlendOp::VK_BLEND_OP_PLUS_CLAMPED_EXT;

				case Flint::ColorBlendOperator::PlusClampedAlpha:
					return VkBlendOp::VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT;

				case Flint::ColorBlendOperator::PlusDarker:
					return VkBlendOp::VK_BLEND_OP_PLUS_DARKER_EXT;

				case Flint::ColorBlendOperator::Minus:
					return VkBlendOp::VK_BLEND_OP_MINUS_EXT;

				case Flint::ColorBlendOperator::MinusClamped:
					return VkBlendOp::VK_BLEND_OP_MINUS_CLAMPED_EXT;

				case Flint::ColorBlendOperator::Contrast:
					return VkBlendOp::VK_BLEND_OP_CONTRAST_EXT;

				case Flint::ColorBlendOperator::InvertOVG:
					return VkBlendOp::VK_BLEND_OP_INVERT_OVG_EXT;

				case Flint::ColorBlendOperator::Red:
					return VkBlendOp::VK_BLEND_OP_RED_EXT;

				case Flint::ColorBlendOperator::Green:
					return VkBlendOp::VK_BLEND_OP_GREEN_EXT;

				case Flint::ColorBlendOperator::Blue:
					return VkBlendOp::VK_BLEND_OP_BLUE_EXT;

				default:
					throw backend_error("Invalid color blend operator!");
				}

				return VkBlendOp::VK_BLEND_OP_ADD;
			}

			VkColorComponentFlags GetComponentFlags(ColorWriteMask mask)
			{
				VkColorComponentFlags flags = 0;

				if ((mask & ColorWriteMask::R) == ColorWriteMask::R)
					flags |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT;

				if ((mask & ColorWriteMask::G) == ColorWriteMask::G)
					flags |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT;

				if ((mask & ColorWriteMask::B) == ColorWriteMask::B)
					flags |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;

				if ((mask & ColorWriteMask::A) == ColorWriteMask::A)
					flags |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;

				return flags;
			}

			VkSampleCountFlags GetSampleCount(MultiSampleCount samples)
			{
				VkSampleCountFlags flags = 0;

				if ((samples & MultiSampleCount::One) == MultiSampleCount::One)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

				if ((samples & MultiSampleCount::Two) == MultiSampleCount::Two)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;

				if ((samples & MultiSampleCount::Four) == MultiSampleCount::Four)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;

				if ((samples & MultiSampleCount::Eight) == MultiSampleCount::Eight)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;

				if ((samples & MultiSampleCount::Sixteen) == MultiSampleCount::Sixteen)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;

				if ((samples & MultiSampleCount::ThirtyTwo) == MultiSampleCount::ThirtyTwo)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;

				if ((samples & MultiSampleCount::SixtyFour) == MultiSampleCount::SixtyFour)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;

				return flags;
			}
		}

		VulkanGraphicsPipeline::VulkanGraphicsPipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pTessellationControlShader, const std::shared_ptr<Shader>& pTessellationEvaluationShader, const std::shared_ptr<Shader>& pGeometryShader, const std::shared_ptr<Shader>& pFragmentShader, const GraphicsPipelineSpecification& specification)
			: GraphicsPipeline(pDevice, pipelineName, pScreenBoundRenderTarget, pVertexShader, pTessellationControlShader, pTessellationEvaluationShader, pGeometryShader, pFragmentShader, specification)
		{
			OPTICK_EVENT();

			vRenderPass = pScreenBoundRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>().GetRenderPass();

			SetupDefaults();
			CreatePipelineLayout();
			CreatePipelineCache();
			CreatePipeline();
		}

		VulkanGraphicsPipeline::VulkanGraphicsPipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pTessellationControlShader, const std::shared_ptr<Shader>& pTessellationEvaluationShader, const std::shared_ptr<Shader>& pGeometryShader, const std::shared_ptr<Shader>& pFragmentShader, const GraphicsPipelineSpecification& specification)
			: GraphicsPipeline(pDevice, pipelineName, pOffScreenRenderTarget, pVertexShader, pTessellationControlShader, pTessellationEvaluationShader, pGeometryShader, pFragmentShader, specification)
		{
			OPTICK_EVENT();

			vRenderPass = pOffScreenRenderTarget->StaticCast<VulkanOffScreenRenderTarget>().GetRenderPass();

			SetupDefaults();
			CreatePipelineLayout();
			CreatePipelineCache();
			CreatePipeline();
		}

		void VulkanGraphicsPipeline::ReloadShaders()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			vDevice.GetDeviceTable().vkDestroyPipeline(vDevice.GetLogicalDevice(), vPipeline, nullptr);

			for (const auto vLayout : vDescriptorSetLayouts)
				vDevice.GetDeviceTable().vkDestroyDescriptorSetLayout(vDevice.GetLogicalDevice(), vLayout, nullptr);

			vDescriptorSetLayouts.clear();
			pResourcePackagers.clear();

			vDevice.GetDeviceTable().vkDestroyPipelineLayout(vDevice.GetLogicalDevice(), vPipelineLayout, nullptr);

			bShouldPrepareResources = true;

			SetupDefaults();
			CreatePipelineLayout();
			CreatePipelineCache();
			CreatePipeline();
			CreateResourcePackagers();
		}

		void VulkanGraphicsPipeline::Recreate(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget)
		{
			OPTICK_EVENT();

			vRenderPass = pScreenBoundRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>().GetRenderPass();

			vkDestroyPipeline(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vPipeline, nullptr);
			CreatePipeline();
		}

		void VulkanGraphicsPipeline::CreateResourcePackagers()
		{
			for (uint32 i = 0; i < vDescriptorSetLayouts.size(); i++)
				pResourcePackagers.emplace_back(std::make_shared<VulkanResourcePackager>(i, shared_from_this(), vDescriptorSetLayouts[i]));
		}

		void VulkanGraphicsPipeline::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			// Write cache data.
			uint64 cacheSize = 0;
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkGetPipelineCacheData(vDevice.GetLogicalDevice(), vPipelineCache, &cacheSize, nullptr));

			unsigned char* pDataStore = new unsigned char[cacheSize];
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkGetPipelineCacheData(vDevice.GetLogicalDevice(), vPipelineCache, &cacheSize, pDataStore));

			WriteDataToCacheFile(cacheSize, pDataStore);
			delete[] pDataStore;

			// Delete pipeline objects.
			vDevice.GetDeviceTable().vkDestroyPipeline(vDevice.GetLogicalDevice(), vPipeline, nullptr);
			vDevice.GetDeviceTable().vkDestroyPipelineLayout(vDevice.GetLogicalDevice(), vPipelineLayout, nullptr);
			vDevice.GetDeviceTable().vkDestroyPipelineCache(vDevice.GetLogicalDevice(), vPipelineCache, nullptr);

			for (const auto vLayout : vDescriptorSetLayouts)
				vDevice.GetDeviceTable().vkDestroyDescriptorSetLayout(vDevice.GetLogicalDevice(), vLayout, nullptr);

			vDescriptorSetLayouts.clear();
			pResourcePackagers.clear();

			bIsTerminated = true;
		}

		void VulkanGraphicsPipeline::SetupDefaults()
		{
			// Resolve shader stages.
			vShaderStageCreateInfo.emplace_back(pVertexShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pFragmentShader)
				vShaderStageCreateInfo.emplace_back(pFragmentShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pTessellationControlShader)
				vShaderStageCreateInfo.emplace_back(pTessellationControlShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pTessellationEvaluationShader)
				vShaderStageCreateInfo.emplace_back(pTessellationEvaluationShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pGeometryShader)
				vShaderStageCreateInfo.emplace_back(pGeometryShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			// Resolve vertex input state.
			{
				VkVertexInputAttributeDescription vAttributeDescription = {};
				VkVertexInputBindingDescription vBindingDescription = {};

				for (const auto binding : mSpecification.mVertexInputAttributeMap)
				{
					vAttributeDescription.binding = binding.first;
					vAttributeDescription.offset = 0;
					for (const auto attribute : binding.second)
					{
						if (attribute.mDataType == ShaderAttributeDataType::BUILT_IN)
							continue;

						vAttributeDescription.location = attribute.mLocation;
						vAttributeDescription.format = Helpers::GetFormatFromSize(attribute.mDataType);

						vVertexAttributes.emplace_back(vAttributeDescription);
						vAttributeDescription.offset += static_cast<uint32>(attribute.mDataType);
					}

					vBindingDescription.binding = binding.first;
					vBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
					vBindingDescription.stride = vAttributeDescription.offset;
					vVertexBindings.emplace_back(vBindingDescription);
				}
			}

			vVertexInputStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vVertexInputStateCreateInfo.pNext = VK_NULL_HANDLE;
			vVertexInputStateCreateInfo.flags = 0;
			vVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(vVertexAttributes.size());
			vVertexInputStateCreateInfo.pVertexAttributeDescriptions = vVertexAttributes.data();
			vVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32>(vVertexBindings.size());
			vVertexInputStateCreateInfo.pVertexBindingDescriptions = vVertexBindings.data();

			// Input assembly state.
			vInputAssemblyStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			vInputAssemblyStateCreateInfo.pNext = VK_NULL_HANDLE;
			vInputAssemblyStateCreateInfo.flags = 0;
			vInputAssemblyStateCreateInfo.primitiveRestartEnable = GET_VK_BOOL(mSpecification.bEnablePrimitiveRestart);
			vInputAssemblyStateCreateInfo.topology = Helpers::GetPrimitiveTopology(mSpecification.mPrimitiveTopology);

			// Tessellation state.
			vTessellationStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			vTessellationStateCreateInfo.pNext = VK_NULL_HANDLE;
			vTessellationStateCreateInfo.flags = 0;
			vTessellationStateCreateInfo.patchControlPoints = mSpecification.mTessellationPatchControlPoints;

			// Color blend state.
			for (const auto attachment : mSpecification.mColorBlendAttachments)
			{
				VkPipelineColorBlendAttachmentState vAttachmentState = {};
				vAttachmentState.blendEnable = GET_VK_BOOL(attachment.mEnableBlend);
				vAttachmentState.alphaBlendOp = Helpers::GetBlendOp(attachment.mAlphaBlendOperator);
				vAttachmentState.colorBlendOp = Helpers::GetBlendOp(attachment.mBlendOperator);
				vAttachmentState.colorWriteMask = Helpers::GetComponentFlags(attachment.mColorWriteMask);
				vAttachmentState.srcColorBlendFactor = Helpers::GetBlendFactor(attachment.mSrcBlendFactor);
				vAttachmentState.srcAlphaBlendFactor = Helpers::GetBlendFactor(attachment.mSrcAlphaBlendFactor);
				vAttachmentState.dstAlphaBlendFactor = Helpers::GetBlendFactor(attachment.mDstAlphaBlendFactor);
				vAttachmentState.dstColorBlendFactor = Helpers::GetBlendFactor(attachment.mDstBlendFactor);

				vCBASS.emplace_back(vAttachmentState);
			}

			vColorBlendStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			vColorBlendStateCreateInfo.pNext = VK_NULL_HANDLE;
			vColorBlendStateCreateInfo.flags = 0;
			vColorBlendStateCreateInfo.logicOp = Helpers::GetLogicOp(mSpecification.mColorBlendLogic);
			vColorBlendStateCreateInfo.logicOpEnable = GET_VK_BOOL(mSpecification.bEnableColorBlendLogic);
			std::copy_n(mSpecification.mColorBlendConstants, 4, vColorBlendStateCreateInfo.blendConstants);

			vColorBlendStateCreateInfo.attachmentCount = static_cast<uint32>(vCBASS.size());
			vColorBlendStateCreateInfo.pAttachments = vCBASS.data();

			// Rasterization state.
			vRasterizationStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			vRasterizationStateCreateInfo.pNext = VK_NULL_HANDLE;
			vRasterizationStateCreateInfo.flags = 0;
			vRasterizationStateCreateInfo.cullMode = Helpers::GetCullMode(mSpecification.mCullMode);
			vRasterizationStateCreateInfo.depthBiasEnable = GET_VK_BOOL(mSpecification.bEnableDepthBias);
			vRasterizationStateCreateInfo.depthBiasClamp = mSpecification.mDepthBiasFactor;
			vRasterizationStateCreateInfo.depthBiasConstantFactor = mSpecification.mDepthConstantFactor;
			vRasterizationStateCreateInfo.depthBiasSlopeFactor = mSpecification.mDepthSlopeFactor;
			vRasterizationStateCreateInfo.depthClampEnable = GET_VK_BOOL(mSpecification.bEnableDepthClamp);
			vRasterizationStateCreateInfo.frontFace = Helpers::GetFrontFace(mSpecification.mFrontFace);
			vRasterizationStateCreateInfo.lineWidth = mSpecification.mRasterizerLineWidth;
			vRasterizationStateCreateInfo.polygonMode = Helpers::GetPolygonMode(mSpecification.mPolygonMode);
			vRasterizationStateCreateInfo.rasterizerDiscardEnable = GET_VK_BOOL(mSpecification.bEnableRasterizerDiscard);

			// Multisample state.
			vMultisampleStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			vMultisampleStateCreateInfo.pNext = VK_NULL_HANDLE;
			vMultisampleStateCreateInfo.flags = 0;
			vMultisampleStateCreateInfo.alphaToCoverageEnable = GET_VK_BOOL(mSpecification.bEnableAlphaCoverage);
			vMultisampleStateCreateInfo.alphaToOneEnable = GET_VK_BOOL(mSpecification.bEnableAlphaToOne);
			vMultisampleStateCreateInfo.minSampleShading = mSpecification.mMinSampleShading;
			vMultisampleStateCreateInfo.pSampleMask;	// TODO
			vMultisampleStateCreateInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(Helpers::GetSampleCount(mSpecification.mRasterizationSamples));
			vMultisampleStateCreateInfo.sampleShadingEnable = GET_VK_BOOL(mSpecification.bEnableSampleShading);

			// Depth stencil state.
			vDepthStencilStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			vDepthStencilStateCreateInfo.pNext = VK_NULL_HANDLE;
			vDepthStencilStateCreateInfo.flags = 0;
			vDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			vDepthStencilStateCreateInfo.depthTestEnable = GET_VK_BOOL(mSpecification.bEnableDepthTest);
			vDepthStencilStateCreateInfo.depthWriteEnable = GET_VK_BOOL(mSpecification.bEnableDepthWrite);
			vDepthStencilStateCreateInfo.depthCompareOp = Helpers::GetCompareOp(mSpecification.mDepthCompareLogic);

			// Dynamic state.
			vDynamicStates = std::move(Helpers::GetDynamicStates(mSpecification.mDynamicStateFlags));

			vDynamicStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			vDynamicStateCreateInfo.pNext = VK_NULL_HANDLE;
			vDynamicStateCreateInfo.flags = 0;
			vDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32>(vDynamicStates.size());
			vDynamicStateCreateInfo.pDynamicStates = vDynamicStates.data();
		}

		void VulkanGraphicsPipeline::CreatePipelineCache()
		{
			OPTICK_EVENT();

			const auto [size, pData] = ReadDataFromCacheFile();

			VkPipelineCacheCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.initialDataSize = size;
			vCreateInfo.pInitialData = pData;

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreatePipelineCache(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vPipelineCache));

			delete[] pData;
		}

		void VulkanGraphicsPipeline::CreatePipelineLayout()
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			std::vector<VkPushConstantRange> vConstantRanges;
			std::unordered_map<uint32, DescriptorSetInfo> descriptorSetInfos;

			// Resolve vertex shader data.
			{
				auto& vShader = pVertexShader->StaticCast<VulkanShader>();
				Utilities::AddPushConstantRangesToVector(vConstantRanges, vShader);

				const auto map = vShader.GetDescriptorSetMap();
				for (const auto entry : map)
				{
					auto& info = descriptorSetInfos[entry.first];
					info.mLayoutBindings.insert(info.mLayoutBindings.end(), entry.second.mLayoutBindings.begin(), entry.second.mLayoutBindings.end());
					info.mPoolSizes.insert(info.mPoolSizes.end(), entry.second.mPoolSizes.begin(), entry.second.mPoolSizes.end());
				}
			}

			// Check and resolve fragment shader data.
			if (pFragmentShader)
			{
				auto& vShader = pFragmentShader->StaticCast<VulkanShader>();
				Utilities::AddPushConstantRangesToVector(vConstantRanges, vShader);

				const auto map = vShader.GetDescriptorSetMap();
				for (const auto entry : map)
				{
					auto& info = descriptorSetInfos[entry.first];
					info.mLayoutBindings.insert(info.mLayoutBindings.end(), entry.second.mLayoutBindings.begin(), entry.second.mLayoutBindings.end());
					info.mPoolSizes.insert(info.mPoolSizes.end(), entry.second.mPoolSizes.begin(), entry.second.mPoolSizes.end());
				}
			}

			// Check and resolve tessellation control shader data.
			if (pTessellationControlShader)
			{
				auto& vShader = pTessellationControlShader->StaticCast<VulkanShader>();
				Utilities::AddPushConstantRangesToVector(vConstantRanges, vShader);

				const auto map = vShader.GetDescriptorSetMap();
				for (const auto entry : map)
				{
					auto& info = descriptorSetInfos[entry.first];
					info.mLayoutBindings.insert(info.mLayoutBindings.end(), entry.second.mLayoutBindings.begin(), entry.second.mLayoutBindings.end());
					info.mPoolSizes.insert(info.mPoolSizes.end(), entry.second.mPoolSizes.begin(), entry.second.mPoolSizes.end());
				}
			}

			// Check and resolve tessellation evaluation shader data.
			if (pTessellationEvaluationShader)
			{
				auto& vShader = pTessellationEvaluationShader->StaticCast<VulkanShader>();
				Utilities::AddPushConstantRangesToVector(vConstantRanges, vShader);

				const auto map = vShader.GetDescriptorSetMap();
				for (const auto entry : map)
				{
					auto& info = descriptorSetInfos[entry.first];
					info.mLayoutBindings.insert(info.mLayoutBindings.end(), entry.second.mLayoutBindings.begin(), entry.second.mLayoutBindings.end());
					info.mPoolSizes.insert(info.mPoolSizes.end(), entry.second.mPoolSizes.begin(), entry.second.mPoolSizes.end());
				}
			}

			// Check and resolve geometry shader data.
			if (pGeometryShader)
			{
				auto& vShader = pGeometryShader->StaticCast<VulkanShader>();
				Utilities::AddPushConstantRangesToVector(vConstantRanges, vShader);

				const auto map = vShader.GetDescriptorSetMap();
				for (const auto entry : map)
				{
					auto& info = descriptorSetInfos[entry.first];
					info.mLayoutBindings.insert(info.mLayoutBindings.end(), entry.second.mLayoutBindings.begin(), entry.second.mLayoutBindings.end());
					info.mPoolSizes.insert(info.mPoolSizes.end(), entry.second.mPoolSizes.begin(), entry.second.mPoolSizes.end());
				}
			}

			// Create descriptor set layouts.
			for (const auto info : descriptorSetInfos)
			{
				VkDescriptorSetLayoutCreateInfo vLayoutCreateInfo = {};
				vLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				vLayoutCreateInfo.pNext = VK_NULL_HANDLE;
				vLayoutCreateInfo.flags = 0;
				vLayoutCreateInfo.bindingCount = static_cast<uint32>(info.second.mLayoutBindings.size());
				vLayoutCreateInfo.pBindings = info.second.mLayoutBindings.data();

				VkDescriptorSetLayout vDescriptorSetLayout = VK_NULL_HANDLE;
				FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateDescriptorSetLayout(vDevice.GetLogicalDevice(), &vLayoutCreateInfo, nullptr, &vDescriptorSetLayout));

				vDescriptorSetLayouts.emplace_back(vDescriptorSetLayout);
			}

			// Create pipeline layout.
			VkPipelineLayoutCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.pushConstantRangeCount = static_cast<uint32>(vConstantRanges.size());
			vCreateInfo.pPushConstantRanges = vConstantRanges.data();
			vCreateInfo.setLayoutCount = static_cast<uint32>(vDescriptorSetLayouts.size());
			vCreateInfo.pSetLayouts = vDescriptorSetLayouts.data();

			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreatePipelineLayout(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vPipelineLayout));
		}

		void VulkanGraphicsPipeline::CreatePipeline()
		{
			OPTICK_EVENT();

			// Resolve viewport state.
			VkRect2D vR2D = {};
			vR2D.extent.width = static_cast<uint32>(pRenderTarget->GetExtent().mWidth);
			vR2D.extent.height = static_cast<uint32>(pRenderTarget->GetExtent().mHeight);
			vR2D.offset = { 0, 0 };

			VkViewport vVP = {};
			vVP.width = static_cast<float>(vR2D.extent.width);
			vVP.height = static_cast<float>(vR2D.extent.height);
			vVP.maxDepth = 1.0f;
			vVP.minDepth = 0.0f;
			vVP.x = 0.0f;
			vVP.y = 0.0f;

			VkPipelineViewportStateCreateInfo vViewportStateCreateInfo = {};
			vViewportStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			vViewportStateCreateInfo.pNext = VK_NULL_HANDLE;
			vViewportStateCreateInfo.flags = 0;
			vViewportStateCreateInfo.scissorCount = 1;
			vViewportStateCreateInfo.pScissors = &vR2D;
			vViewportStateCreateInfo.viewportCount = 1;
			vViewportStateCreateInfo.pViewports = &vVP;

			// Pipeline create info.
			VkGraphicsPipelineCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.stageCount = static_cast<uint32>(vShaderStageCreateInfo.size());
			vCreateInfo.pStages = vShaderStageCreateInfo.data();
			vCreateInfo.pVertexInputState = &vVertexInputStateCreateInfo;
			vCreateInfo.pInputAssemblyState = &vInputAssemblyStateCreateInfo;
			vCreateInfo.pTessellationState = &vTessellationStateCreateInfo;
			vCreateInfo.pViewportState = &vViewportStateCreateInfo;
			vCreateInfo.pRasterizationState = &vRasterizationStateCreateInfo;
			vCreateInfo.pMultisampleState = &vMultisampleStateCreateInfo;
			vCreateInfo.pDepthStencilState = &vDepthStencilStateCreateInfo;
			vCreateInfo.pColorBlendState = &vColorBlendStateCreateInfo;
			vCreateInfo.pDynamicState = &vDynamicStateCreateInfo;
			vCreateInfo.layout = vPipelineLayout;
			vCreateInfo.renderPass = vRenderPass;
			vCreateInfo.subpass = 0;	// TODO
			vCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			vCreateInfo.basePipelineIndex = 0;

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateGraphicsPipelines(vDevice.GetLogicalDevice(), vPipelineCache, 1, &vCreateInfo, nullptr, &vPipeline));
		}
	}
}