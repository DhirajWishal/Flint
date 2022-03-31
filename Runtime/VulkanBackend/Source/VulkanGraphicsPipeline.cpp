// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

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

		VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice* m_pDevice, const std::string& pipelineName, const VulkanScreenBoundRenderTarget* pScreenBoundRenderTarget, std::unique_ptr<VulkanShader>&& pVertexShader, std::unique_ptr<VulkanShader>&& pTessellationControlShader, std::unique_ptr<VulkanShader>&& pTessellationEvaluationShader, std::unique_ptr<VulkanShader>&& pGeometryShader, std::unique_ptr<VulkanShader>&& pFragmentShader, const GraphicsPipelineSpecification& specification)
			: GraphicsPipeline(m_pDevice, pipelineName, pScreenBoundRenderTarget, std::move(pVertexShader), std::move(pTessellationControlShader), std::move(pTessellationEvaluationShader), std::move(pGeometryShader), std::move(pFragmentShader), specification)
		{
			OPTICK_EVENT();

			m_vRenderPass = pScreenBoundRenderTarget->GetRenderPass();

			SetupDefaults();
			CreatePipelineLayout();
			CreatePipelineCache();
			CreatePipeline();
		}

		VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice* m_pDevice, const std::string& pipelineName, const VulkanOffScreenRenderTarget* pOffScreenRenderTarget, std::unique_ptr<VulkanShader>&& pVertexShader, std::unique_ptr<VulkanShader>&& pTessellationControlShader, std::unique_ptr<VulkanShader>&& pTessellationEvaluationShader, std::unique_ptr<VulkanShader>&& pGeometryShader, std::unique_ptr<VulkanShader>&& pFragmentShader, const GraphicsPipelineSpecification& specification)
			: GraphicsPipeline(m_pDevice, pipelineName, pOffScreenRenderTarget, std::move(pVertexShader), std::move(pTessellationControlShader), std::move(pTessellationEvaluationShader), std::move(pGeometryShader), std::move(pFragmentShader), specification)
		{
			OPTICK_EVENT();

			m_vRenderPass = pOffScreenRenderTarget->GetRenderPass();

			SetupDefaults();
			CreatePipelineLayout();
			CreatePipelineCache();
			CreatePipeline();
		}

		void VulkanGraphicsPipeline::ReloadShaders()
		{
			VulkanDevice& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();

			m_vDevice.GetDeviceTable().vkDestroyPipeline(m_vDevice.GetLogicalDevice(), m_vPipeline, nullptr);

			for (const auto m_vLayout : m_vDescriptorSetLayouts)
				m_vDevice.GetDeviceTable().vkDestroyDescriptorSetLayout(m_vDevice.GetLogicalDevice(), m_vLayout, nullptr);

			m_vDescriptorSetLayouts.clear();
			pResourcePackagers.clear();

			m_vDevice.GetDeviceTable().vkDestroyPipelineLayout(m_vDevice.GetLogicalDevice(), m_vPipelineLayout, nullptr);

			bShouldPrepareResources = true;

			SetupDefaults();
			CreatePipelineLayout();
			CreatePipelineCache();
			CreatePipeline();
			CreateResourcePackagers();
		}

		void VulkanGraphicsPipeline::Recreate(VulkanScreenBoundRenderTarget* pScreenBoundRenderTarget)
		{
			OPTICK_EVENT();

			m_vRenderPass = pScreenBoundRenderTarget->GetRenderPass();

			vkDestroyPipeline(m_pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), m_vPipeline, nullptr);
			CreatePipeline();
		}

		void VulkanGraphicsPipeline::CreateResourcePackagers()
		{
			for (uint32_t i = 0; i < m_vDescriptorSetLayouts.size(); i++)
				pResourcePackagers.emplace_back(std::make_shared<VulkanResourcePackager>(i, this, m_vDescriptorSetLayouts[i]));
		}

		void VulkanGraphicsPipeline::Terminate()
		{
			VulkanDevice& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();

			// Write cache data.
			uint64_t cacheSize = 0;
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkGetPipelineCacheData(m_vDevice.GetLogicalDevice(), m_vPipelineCache, &cacheSize, nullptr));

			unsigned char* pDataStore = new unsigned char[cacheSize];
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkGetPipelineCacheData(m_vDevice.GetLogicalDevice(), m_vPipelineCache, &cacheSize, pDataStore));

			WriteDataToCacheFile(cacheSize, pDataStore);
			delete[] pDataStore;

			// Delete pipeline objects.
			m_vDevice.GetDeviceTable().vkDestroyPipeline(m_vDevice.GetLogicalDevice(), m_vPipeline, nullptr);
			m_vDevice.GetDeviceTable().vkDestroyPipelineLayout(m_vDevice.GetLogicalDevice(), m_vPipelineLayout, nullptr);
			m_vDevice.GetDeviceTable().vkDestroyPipelineCache(m_vDevice.GetLogicalDevice(), m_vPipelineCache, nullptr);

			for (const auto m_vLayout : m_vDescriptorSetLayouts)
				m_vDevice.GetDeviceTable().vkDestroyDescriptorSetLayout(m_vDevice.GetLogicalDevice(), m_vLayout, nullptr);

			m_vDescriptorSetLayouts.clear();
			pResourcePackagers.clear();

			bIsTerminated = true;
		}

		void VulkanGraphicsPipeline::SetupDefaults()
		{
			// Resolve shader stages.
			m_vShaderStageCreateInfo.emplace_back(pVertexShader->GetShaderStageCreateInfo());

			if (pFragmentShader)
				m_vShaderStageCreateInfo.emplace_back(pFragmentShader->GetShaderStageCreateInfo());

			if (pTessellationControlShader)
				m_vShaderStageCreateInfo.emplace_back(pTessellationControlShader->GetShaderStageCreateInfo());

			if (pTessellationEvaluationShader)
				m_vShaderStageCreateInfo.emplace_back(pTessellationEvaluationShader->GetShaderStageCreateInfo());

			if (pGeometryShader)
				m_vShaderStageCreateInfo.emplace_back(pGeometryShader->GetShaderStageCreateInfo());

			// Resolve vertex input state.
			{
				VkVertexInputAttributeDescription m_vAttributeDescription = {};
				VkVertexInputBindingDescription m_vBindingDescription = {};

				for (const auto binding : m_Specification.m_VertexInputAttributeMap)
				{
					m_vAttributeDescription.binding = binding.first;
					m_vAttributeDescription.offset = 0;
					for (const auto attribute : binding.second)
					{
						if (attribute.m_DataType == ShaderAttributeDataType::BUILT_IN)
							continue;

						m_vAttributeDescription.location = attribute.m_Location;
						m_vAttributeDescription.format = Helpers::GetFormatFromSize(attribute.m_DataType);

						m_vVertexAttributes.emplace_back(m_vAttributeDescription);
						m_vAttributeDescription.offset += static_cast<uint32_t>(attribute.m_DataType);
					}

					m_vBindingDescription.binding = binding.first;
					m_vBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
					m_vBindingDescription.stride = m_vAttributeDescription.offset;
					m_vVertexBindings.emplace_back(m_vBindingDescription);
				}
			}

			m_vVertexInputStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			m_vVertexInputStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_vVertexInputStateCreateInfo.flags = 0;
			m_vVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vVertexAttributes.size());
			m_vVertexInputStateCreateInfo.pVertexAttributeDescriptions = m_vVertexAttributes.data();
			m_vVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vVertexBindings.size());
			m_vVertexInputStateCreateInfo.pVertexBindingDescriptions = m_vVertexBindings.data();

			// Input assembly state.
			m_vInputAssemblyStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			m_vInputAssemblyStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_vInputAssemblyStateCreateInfo.flags = 0;
			m_vInputAssemblyStateCreateInfo.primitiveRestartEnable = GET_VK_BOOL(m_Specification.bEnablePrimitiveRestart);
			m_vInputAssemblyStateCreateInfo.topology = Helpers::GetPrimitiveTopology(m_Specification.m_PrimitiveTopology);

			// Tessellation state.
			m_vTessellationStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			m_vTessellationStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_vTessellationStateCreateInfo.flags = 0;
			m_vTessellationStateCreateInfo.patchControlPoints = m_Specification.m_TessellationPatchControlPoints;

			// Color blend state.
			for (const auto attachment : m_Specification.m_ColorBlendAttachments)
			{
				VkPipelineColorBlendAttachmentState m_vAttachmentState = {};
				m_vAttachmentState.blendEnable = GET_VK_BOOL(attachment.m_EnableBlend);
				m_vAttachmentState.alphaBlendOp = Helpers::GetBlendOp(attachment.m_AlphaBlendOperator);
				m_vAttachmentState.colorBlendOp = Helpers::GetBlendOp(attachment.m_BlendOperator);
				m_vAttachmentState.colorWriteMask = Helpers::GetComponentFlags(attachment.m_ColorWriteMask);
				m_vAttachmentState.srcColorBlendFactor = Helpers::GetBlendFactor(attachment.m_SrcBlendFactor);
				m_vAttachmentState.srcAlphaBlendFactor = Helpers::GetBlendFactor(attachment.m_SrcAlphaBlendFactor);
				m_vAttachmentState.dstAlphaBlendFactor = Helpers::GetBlendFactor(attachment.m_DstAlphaBlendFactor);
				m_vAttachmentState.dstColorBlendFactor = Helpers::GetBlendFactor(attachment.m_DstBlendFactor);

				m_vCBASS.emplace_back(m_vAttachmentState);
			}

			m_vColorBlendStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			m_vColorBlendStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_vColorBlendStateCreateInfo.flags = 0;
			m_vColorBlendStateCreateInfo.logicOp = Helpers::GetLogicOp(m_Specification.m_ColorBlendLogic);
			m_vColorBlendStateCreateInfo.logicOpEnable = GET_VK_BOOL(m_Specification.bEnableColorBlendLogic);
			std::copy_n(m_Specification.m_ColorBlendConstants, 4, m_vColorBlendStateCreateInfo.blendConstants);

			m_vColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(m_vCBASS.size());
			m_vColorBlendStateCreateInfo.pAttachments = m_vCBASS.data();

			// Rasterization state.
			m_vRasterizationStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			m_vRasterizationStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_vRasterizationStateCreateInfo.flags = 0;
			m_vRasterizationStateCreateInfo.cullMode = Helpers::GetCullMode(m_Specification.m_CullMode);
			m_vRasterizationStateCreateInfo.depthBiasEnable = GET_VK_BOOL(m_Specification.bEnableDepthBias);
			m_vRasterizationStateCreateInfo.depthBiasClamp = m_Specification.m_DepthBiasFactor;
			m_vRasterizationStateCreateInfo.depthBiasConstantFactor = m_Specification.m_DepthConstantFactor;
			m_vRasterizationStateCreateInfo.depthBiasSlopeFactor = m_Specification.m_DepthSlopeFactor;
			m_vRasterizationStateCreateInfo.depthClampEnable = GET_VK_BOOL(m_Specification.bEnableDepthClamp);
			m_vRasterizationStateCreateInfo.frontFace = Helpers::GetFrontFace(m_Specification.m_FrontFace);
			m_vRasterizationStateCreateInfo.lineWidth = m_Specification.m_RasterizerLineWidth;
			m_vRasterizationStateCreateInfo.polygonMode = Helpers::GetPolygonMode(m_Specification.m_PolygonMode);
			m_vRasterizationStateCreateInfo.rasterizerDiscardEnable = GET_VK_BOOL(m_Specification.bEnableRasterizerDiscard);

			// Multisample state.
			m_vMultisampleStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			m_vMultisampleStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_vMultisampleStateCreateInfo.flags = 0;
			m_vMultisampleStateCreateInfo.alphaToCoverageEnable = GET_VK_BOOL(m_Specification.bEnableAlphaCoverage);
			m_vMultisampleStateCreateInfo.alphaToOneEnable = GET_VK_BOOL(m_Specification.bEnableAlphaToOne);
			m_vMultisampleStateCreateInfo.minSampleShading = m_Specification.m_MinSampleShading;
			m_vMultisampleStateCreateInfo.pSampleMask;	// TODO
			m_vMultisampleStateCreateInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(Helpers::GetSampleCount(m_Specification.m_RasterizationSamples));
			m_vMultisampleStateCreateInfo.sampleShadingEnable = GET_VK_BOOL(m_Specification.bEnableSampleShading);

			// Depth stencil state.
			m_vDepthStencilStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			m_vDepthStencilStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_vDepthStencilStateCreateInfo.flags = 0;
			m_vDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			m_vDepthStencilStateCreateInfo.depthTestEnable = GET_VK_BOOL(m_Specification.bEnableDepthTest);
			m_vDepthStencilStateCreateInfo.depthWriteEnable = GET_VK_BOOL(m_Specification.bEnableDepthWrite);
			m_vDepthStencilStateCreateInfo.depthCompareOp = Helpers::GetCompareOp(m_Specification.m_DepthCompareLogic);

			// Dynamic state.
			m_vDynamicStates = std::move(Helpers::GetDynamicStates(m_Specification.m_DynamicStateFlags));

			m_vDynamicStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			m_vDynamicStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_vDynamicStateCreateInfo.flags = 0;
			m_vDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(m_vDynamicStates.size());
			m_vDynamicStateCreateInfo.pDynamicStates = m_vDynamicStates.data();
		}

		void VulkanGraphicsPipeline::CreatePipelineCache()
		{
			OPTICK_EVENT();

			const auto [size, pData] = ReadDataFromCacheFile();

			VkPipelineCacheCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.initialDataSize = size;
			m_vCreateInfo.pInitialData = pData;

			VulkanDevice& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreatePipelineCache(m_vDevice.GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vPipelineCache));

			delete[] pData;
		}

		void VulkanGraphicsPipeline::CreatePipelineLayout()
		{
			OPTICK_EVENT();

			VulkanDevice& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();
			std::vector<VkPushConstantRange> m_vConstantRanges;
			std::unordered_map<uint32_t, DescriptorSetInfo> descriptorSetInfos;

			// Resolve vertex shader data.
			{
				Utilities::AddPushConstantRangesToVector(m_vConstantRanges, *pVertexShader);

				const auto map = pVertexShader->GetDescriptorSetMap();
				for (const auto entry : map)
				{
					auto& info = descriptorSetInfos[entry.first];
					info.m_LayoutBindings.insert(info.m_LayoutBindings.end(), entry.second.m_LayoutBindings.begin(), entry.second.m_LayoutBindings.end());
					info.m_PoolSizes.insert(info.m_PoolSizes.end(), entry.second.m_PoolSizes.begin(), entry.second.m_PoolSizes.end());
				}
			}

			// Check and resolve fragment shader data.
			if (pFragmentShader)
			{
				Utilities::AddPushConstantRangesToVector(m_vConstantRanges, *pFragmentShader);

				const auto map = pFragmentShader->GetDescriptorSetMap();
				for (const auto entry : map)
				{
					auto& info = descriptorSetInfos[entry.first];
					info.m_LayoutBindings.insert(info.m_LayoutBindings.end(), entry.second.m_LayoutBindings.begin(), entry.second.m_LayoutBindings.end());
					info.m_PoolSizes.insert(info.m_PoolSizes.end(), entry.second.m_PoolSizes.begin(), entry.second.m_PoolSizes.end());
				}
			}

			// Check and resolve tessellation control shader data.
			if (pTessellationControlShader)
			{
				Utilities::AddPushConstantRangesToVector(m_vConstantRanges, *pTessellationControlShader);

				const auto map = pTessellationControlShader->GetDescriptorSetMap();
				for (const auto entry : map)
				{
					auto& info = descriptorSetInfos[entry.first];
					info.m_LayoutBindings.insert(info.m_LayoutBindings.end(), entry.second.m_LayoutBindings.begin(), entry.second.m_LayoutBindings.end());
					info.m_PoolSizes.insert(info.m_PoolSizes.end(), entry.second.m_PoolSizes.begin(), entry.second.m_PoolSizes.end());
				}
			}

			// Check and resolve tessellation evaluation shader data.
			if (pTessellationEvaluationShader)
			{
				Utilities::AddPushConstantRangesToVector(m_vConstantRanges, *pTessellationEvaluationShader);

				const auto map = pTessellationEvaluationShader->GetDescriptorSetMap();
				for (const auto entry : map)
				{
					auto& info = descriptorSetInfos[entry.first];
					info.m_LayoutBindings.insert(info.m_LayoutBindings.end(), entry.second.m_LayoutBindings.begin(), entry.second.m_LayoutBindings.end());
					info.m_PoolSizes.insert(info.m_PoolSizes.end(), entry.second.m_PoolSizes.begin(), entry.second.m_PoolSizes.end());
				}
			}

			// Check and resolve geometry shader data.
			if (pGeometryShader)
			{
				Utilities::AddPushConstantRangesToVector(m_vConstantRanges, *pGeometryShader);

				const auto map = pGeometryShader->GetDescriptorSetMap();
				for (const auto entry : map)
				{
					auto& info = descriptorSetInfos[entry.first];
					info.m_LayoutBindings.insert(info.m_LayoutBindings.end(), entry.second.m_LayoutBindings.begin(), entry.second.m_LayoutBindings.end());
					info.m_PoolSizes.insert(info.m_PoolSizes.end(), entry.second.m_PoolSizes.begin(), entry.second.m_PoolSizes.end());
				}
			}

			// Create descriptor set layouts.
			for (const auto info : descriptorSetInfos)
			{
				VkDescriptorSetLayoutCreateInfo m_vLayoutCreateInfo = {};
				m_vLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				m_vLayoutCreateInfo.pNext = VK_NULL_HANDLE;
				m_vLayoutCreateInfo.flags = 0;
				m_vLayoutCreateInfo.bindingCount = static_cast<uint32_t>(info.second.m_LayoutBindings.size());
				m_vLayoutCreateInfo.pBindings = info.second.m_LayoutBindings.data();

				VkDescriptorSetLayout m_vDescriptorSetLayout = VK_NULL_HANDLE;
				FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateDescriptorSetLayout(m_vDevice.GetLogicalDevice(), &m_vLayoutCreateInfo, nullptr, &m_vDescriptorSetLayout));

				m_vDescriptorSetLayouts.emplace_back(m_vDescriptorSetLayout);
			}

			// Create pipeline layout.
			VkPipelineLayoutCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(m_vConstantRanges.size());
			m_vCreateInfo.pPushConstantRanges = m_vConstantRanges.data();
			m_vCreateInfo.setLayoutCount = static_cast<uint32_t>(m_vDescriptorSetLayouts.size());
			m_vCreateInfo.pSetLayouts = m_vDescriptorSetLayouts.data();

			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreatePipelineLayout(m_vDevice.GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vPipelineLayout));
		}

		void VulkanGraphicsPipeline::CreatePipeline()
		{
			OPTICK_EVENT();

			// Resolve viewport state.
			VkRect2D m_vR2D = {};
			m_vR2D.extent.width = static_cast<uint32_t>(pRenderTarget->GetExtent().m_Width);
			m_vR2D.extent.height = static_cast<uint32_t>(pRenderTarget->GetExtent().m_Height);
			m_vR2D.offset = { 0, 0 };

			VkViewport m_vVP = {};
			m_vVP.width = static_cast<float>(m_vR2D.extent.width);
			m_vVP.height = static_cast<float>(m_vR2D.extent.height);
			m_vVP.maxDepth = 1.0f;
			m_vVP.minDepth = 0.0f;
			m_vVP.x = 0.0f;
			m_vVP.y = 0.0f;

			VkPipelineViewportStateCreateInfo m_vViewportStateCreateInfo = {};
			m_vViewportStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			m_vViewportStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_vViewportStateCreateInfo.flags = 0;
			m_vViewportStateCreateInfo.scissorCount = 1;
			m_vViewportStateCreateInfo.pScissors = &m_vR2D;
			m_vViewportStateCreateInfo.viewportCount = 1;
			m_vViewportStateCreateInfo.pViewports = &m_vVP;

			// Pipeline create info.
			VkGraphicsPipelineCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.stageCount = static_cast<uint32_t>(m_vShaderStageCreateInfo.size());
			m_vCreateInfo.pStages = m_vShaderStageCreateInfo.data();
			m_vCreateInfo.pVertexInputState = &m_vVertexInputStateCreateInfo;
			m_vCreateInfo.pInputAssemblyState = &m_vInputAssemblyStateCreateInfo;
			m_vCreateInfo.pTessellationState = &m_vTessellationStateCreateInfo;
			m_vCreateInfo.pViewportState = &m_vViewportStateCreateInfo;
			m_vCreateInfo.pRasterizationState = &m_vRasterizationStateCreateInfo;
			m_vCreateInfo.pMultisampleState = &m_vMultisampleStateCreateInfo;
			m_vCreateInfo.pDepthStencilState = &m_vDepthStencilStateCreateInfo;
			m_vCreateInfo.pColorBlendState = &m_vColorBlendStateCreateInfo;
			m_vCreateInfo.pDynamicState = &m_vDynamicStateCreateInfo;
			m_vCreateInfo.layout = m_vPipelineLayout;
			m_vCreateInfo.renderPass = m_vRenderPass;
			m_vCreateInfo.subpass = 0;	// TODO
			m_vCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			m_vCreateInfo.basePipelineIndex = 0;

			VulkanDevice& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateGraphicsPipelines(m_vDevice.GetLogicalDevice(), m_vPipelineCache, 1, &m_vCreateInfo, nullptr, &m_vPipeline));
		}
	}
}