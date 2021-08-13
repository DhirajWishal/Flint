// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanShader.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanImage.hpp"
#include "VulkanBackend/VulkanImageSampler.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
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
					FLINT_THROW_BACKEND_ERROR("Invalid or unsupported shader attribute type!");
				}

				return VkFormat::VK_FORMAT_UNDEFINED;
			}

			VkPrimitiveTopology GetPrimitiveTopology(PrimitiveTopology topology)
			{
				switch (topology)
				{
				case Flint::PrimitiveTopology::POINT_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

				case Flint::PrimitiveTopology::LINE_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

				case Flint::PrimitiveTopology::LINE_STRIP:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

				case Flint::PrimitiveTopology::TRIANGLE_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

				case Flint::PrimitiveTopology::TRIANGLE_STRIP:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

				case Flint::PrimitiveTopology::TRIANGLE_FAN:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

				case Flint::PrimitiveTopology::LINE_LIST_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::PATCH_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined primitive topology!");
				}

				return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			}

			VkCullModeFlags GetCullMode(CullMode cull)
			{
				switch (cull)
				{
				case Flint::CullMode::NONE:
					return VkCullModeFlagBits::VK_CULL_MODE_NONE;

				case Flint::CullMode::FRONT:
					return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;

				case Flint::CullMode::BACK:
					return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;

				case Flint::CullMode::FRONT_AND_BACK:
					return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined cull mode!");
				}

				return VkCullModeFlagBits::VK_CULL_MODE_NONE;
			}

			VkFrontFace GetFrontFace(FrontFace face)
			{
				switch (face)
				{
				case Flint::FrontFace::COUNTER_CLOCKWISE:
					return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;

				case Flint::FrontFace::CLOCKWISE:
					return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined front face!");
				}

				return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
			}

			VkPolygonMode GetPolygonMode(PolygonMode mode)
			{
				switch (mode)
				{
				case Flint::PolygonMode::FILL:
					return VkPolygonMode::VK_POLYGON_MODE_FILL;

				case Flint::PolygonMode::LINE:
					return VkPolygonMode::VK_POLYGON_MODE_LINE;

				case Flint::PolygonMode::POINT:
					return VkPolygonMode::VK_POLYGON_MODE_POINT;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined polygon mode!");
				}

				return VkPolygonMode::VK_POLYGON_MODE_FILL;
			}

			VkLogicOp GetLogicOp(ColorBlendLogic logic)
			{
				switch (logic)
				{
				case Flint::ColorBlendLogic::CLEAR:
					return VkLogicOp::VK_LOGIC_OP_CLEAR;

				case Flint::ColorBlendLogic::AND:
					return VkLogicOp::VK_LOGIC_OP_AND;

				case Flint::ColorBlendLogic::AND_REVERSE:
					return VkLogicOp::VK_LOGIC_OP_AND_REVERSE;

				case Flint::ColorBlendLogic::COPY:
					return VkLogicOp::VK_LOGIC_OP_COPY;

				case Flint::ColorBlendLogic::AND_INVERTED:
					return VkLogicOp::VK_LOGIC_OP_AND_INVERTED;

				case Flint::ColorBlendLogic::NO_OP:
					return VkLogicOp::VK_LOGIC_OP_NO_OP;

				case Flint::ColorBlendLogic::XOR:
					return VkLogicOp::VK_LOGIC_OP_XOR;

				case Flint::ColorBlendLogic::OR:
					return VkLogicOp::VK_LOGIC_OP_OR;

				case Flint::ColorBlendLogic::NOR:
					return VkLogicOp::VK_LOGIC_OP_NOR;

				case Flint::ColorBlendLogic::EQUIVALENT:
					return VkLogicOp::VK_LOGIC_OP_EQUIVALENT;

				case Flint::ColorBlendLogic::INVERT:
					return VkLogicOp::VK_LOGIC_OP_INVERT;

				case Flint::ColorBlendLogic::OR_REVERSE:
					return VkLogicOp::VK_LOGIC_OP_OR_REVERSE;

				case Flint::ColorBlendLogic::COPY_INVERTED:
					return VkLogicOp::VK_LOGIC_OP_COPY_INVERTED;

				case Flint::ColorBlendLogic::OR_INVERTED:
					return VkLogicOp::VK_LOGIC_OP_OR_INVERTED;

				case Flint::ColorBlendLogic::NAND:
					return VkLogicOp::VK_LOGIC_OP_NAND;

				case Flint::ColorBlendLogic::SET:
					return VkLogicOp::VK_LOGIC_OP_SET;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined color blend logic!");
				}

				return VkLogicOp::VK_LOGIC_OP_CLEAR;
			}

			VkCompareOp GetCompareOp(DepthCompareLogic logic)
			{
				switch (logic)
				{
				case Flint::DepthCompareLogic::NEVER:
					return VkCompareOp::VK_COMPARE_OP_NEVER;

				case Flint::DepthCompareLogic::LESS:
					return VkCompareOp::VK_COMPARE_OP_LESS;

				case Flint::DepthCompareLogic::EQUAL:
					return VkCompareOp::VK_COMPARE_OP_EQUAL;

				case Flint::DepthCompareLogic::LESS_OR_EQUAL:
					return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;

				case Flint::DepthCompareLogic::GREATER:
					return VkCompareOp::VK_COMPARE_OP_GREATER;

				case Flint::DepthCompareLogic::NOT_EQUAL:
					return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;

				case Flint::DepthCompareLogic::GREATER_OR_EQUAL:
					return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;

				case Flint::DepthCompareLogic::ALWAYS:
					return VkCompareOp::VK_COMPARE_OP_ALWAYS;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined depth compare logic!");
				}

				return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
			}

			std::vector<VkDynamicState> GetDynamicStates(DynamicStateFlags flags)
			{
				std::vector<VkDynamicState> states;
				if (flags & DynamicStateFlags::VIEWPORT)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT);

				if (flags & DynamicStateFlags::SCISSOR)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_SCISSOR);

				if (flags & DynamicStateFlags::LINE_WIDTH)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_LINE_WIDTH);

				if (flags & DynamicStateFlags::DEPTH_BIAS)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BIAS);

				if (flags & DynamicStateFlags::BLEND_CONSTANTS)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_BLEND_CONSTANTS);

				if (flags & DynamicStateFlags::DEPTH_BOUNDS)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BOUNDS);

				return states;
			}

			VkBlendFactor GetBlendFactor(ColorBlendFactor factor)
			{
				switch (factor)
				{
				case Flint::ColorBlendFactor::ZERO:
					return VkBlendFactor::VK_BLEND_FACTOR_ZERO;

				case Flint::ColorBlendFactor::ONE:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE;

				case Flint::ColorBlendFactor::SRC_COLOR:
					return VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR;

				case Flint::ColorBlendFactor::ONE_MINUS_SRC_COLOR:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

				case Flint::ColorBlendFactor::DST_COLOR:
					return VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR;

				case Flint::ColorBlendFactor::ONE_MINUS_DST_COLOR:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

				case Flint::ColorBlendFactor::SRC_ALPHA:
					return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;

				case Flint::ColorBlendFactor::ONE_MINUS_SRC_ALPHA:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

				case Flint::ColorBlendFactor::DST_ALPHA:
					return VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA;

				case Flint::ColorBlendFactor::ONE_MINUS_DST_ALPHA:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

				case Flint::ColorBlendFactor::CONSTANT_COLOR:
					return VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_COLOR;

				case Flint::ColorBlendFactor::ONE_MINUS_CONSTANT_COLOR:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

				case Flint::ColorBlendFactor::CONSTANT_ALPHA:
					return VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_ALPHA;

				case Flint::ColorBlendFactor::ONE_MINUS_CONSTANT_ALPHA:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;

				case Flint::ColorBlendFactor::SRC_ALPHA_SATURATE:
					return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;

				case Flint::ColorBlendFactor::SRC1_COLOR:
					return VkBlendFactor::VK_BLEND_FACTOR_SRC1_COLOR;

				case Flint::ColorBlendFactor::ONE_MINUS_SRC1_COLOR:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;

				case Flint::ColorBlendFactor::SRC1_ALPHA:
					return VkBlendFactor::VK_BLEND_FACTOR_SRC1_ALPHA;

				case Flint::ColorBlendFactor::ONE_MINUS_SRC1_ALPHA:
					return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid color blend factor!");
				}

				return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
			}

			VkBlendOp GetBlendOp(ColorBlendOperator oprtr)
			{
				switch (oprtr)
				{
				case Flint::ColorBlendOperator::ADD:
					return VkBlendOp::VK_BLEND_OP_ADD;

				case Flint::ColorBlendOperator::SUBTRACT:
					return VkBlendOp::VK_BLEND_OP_SUBTRACT;

				case Flint::ColorBlendOperator::REVERSE_SUBTRACT:
					return VkBlendOp::VK_BLEND_OP_REVERSE_SUBTRACT;

				case Flint::ColorBlendOperator::MIN:
					return VkBlendOp::VK_BLEND_OP_MIN;

				case Flint::ColorBlendOperator::MAX:
					return VkBlendOp::VK_BLEND_OP_MAX;

				case Flint::ColorBlendOperator::ZERO:
					return VkBlendOp::VK_BLEND_OP_ZERO_EXT;

				case Flint::ColorBlendOperator::SRC:
					return VkBlendOp::VK_BLEND_OP_SRC_EXT;

				case Flint::ColorBlendOperator::DST:
					return VkBlendOp::VK_BLEND_OP_DST_EXT;

				case Flint::ColorBlendOperator::SRC_OVER:
					return VkBlendOp::VK_BLEND_OP_SRC_OVER_EXT;

				case Flint::ColorBlendOperator::DST_OVER:
					return VkBlendOp::VK_BLEND_OP_DST_OVER_EXT;

				case Flint::ColorBlendOperator::SRC_IN:
					return VkBlendOp::VK_BLEND_OP_SRC_IN_EXT;

				case Flint::ColorBlendOperator::DST_IN:
					return VkBlendOp::VK_BLEND_OP_DST_IN_EXT;

				case Flint::ColorBlendOperator::SRC_OUT:
					return VkBlendOp::VK_BLEND_OP_SRC_OUT_EXT;

				case Flint::ColorBlendOperator::DST_OUT:
					return VkBlendOp::VK_BLEND_OP_DST_OUT_EXT;

				case Flint::ColorBlendOperator::SRC_ATOP:
					return VkBlendOp::VK_BLEND_OP_SRC_ATOP_EXT;

				case Flint::ColorBlendOperator::DST_ATOP:
					return VkBlendOp::VK_BLEND_OP_DST_ATOP_EXT;

				case Flint::ColorBlendOperator::XOR:
					return VkBlendOp::VK_BLEND_OP_XOR_EXT;

				case Flint::ColorBlendOperator::MULTIPLY:
					return VkBlendOp::VK_BLEND_OP_MULTIPLY_EXT;

				case Flint::ColorBlendOperator::SCREEN:
					return VkBlendOp::VK_BLEND_OP_SCREEN_EXT;

				case Flint::ColorBlendOperator::OVERLAY:
					return VkBlendOp::VK_BLEND_OP_OVERLAY_EXT;

				case Flint::ColorBlendOperator::DARKEN:
					return VkBlendOp::VK_BLEND_OP_DARKEN_EXT;

				case Flint::ColorBlendOperator::LIGHTEN:
					return VkBlendOp::VK_BLEND_OP_LIGHTEN_EXT;

				case Flint::ColorBlendOperator::COLORDODGE:
					return VkBlendOp::VK_BLEND_OP_COLORDODGE_EXT;

				case Flint::ColorBlendOperator::COLORBURN:
					return VkBlendOp::VK_BLEND_OP_COLORBURN_EXT;

				case Flint::ColorBlendOperator::HARDLIGHT:
					return VkBlendOp::VK_BLEND_OP_HARDLIGHT_EXT;

				case Flint::ColorBlendOperator::SOFTLIGHT:
					return VkBlendOp::VK_BLEND_OP_SOFTLIGHT_EXT;

				case Flint::ColorBlendOperator::DIFFERENCE:
					return VkBlendOp::VK_BLEND_OP_DIFFERENCE_EXT;

				case Flint::ColorBlendOperator::EXCLUSION:
					return VkBlendOp::VK_BLEND_OP_EXCLUSION_EXT;

				case Flint::ColorBlendOperator::INVERT:
					return VkBlendOp::VK_BLEND_OP_INVERT_EXT;

				case Flint::ColorBlendOperator::INVERT_RGB:
					return VkBlendOp::VK_BLEND_OP_INVERT_RGB_EXT;

				case Flint::ColorBlendOperator::LINEARDODGE:
					return VkBlendOp::VK_BLEND_OP_LINEARDODGE_EXT;

				case Flint::ColorBlendOperator::LINEARBURN:
					return VkBlendOp::VK_BLEND_OP_LINEARBURN_EXT;

				case Flint::ColorBlendOperator::VIVIDLIGHT:
					return VkBlendOp::VK_BLEND_OP_VIVIDLIGHT_EXT;

				case Flint::ColorBlendOperator::LINEARLIGHT:
					return VkBlendOp::VK_BLEND_OP_LINEARLIGHT_EXT;

				case Flint::ColorBlendOperator::PINLIGHT:
					return VkBlendOp::VK_BLEND_OP_PINLIGHT_EXT;

				case Flint::ColorBlendOperator::HARDMIX:
					return VkBlendOp::VK_BLEND_OP_HARDMIX_EXT;

				case Flint::ColorBlendOperator::HSL_HUE:
					return VkBlendOp::VK_BLEND_OP_HSL_HUE_EXT;

				case Flint::ColorBlendOperator::HSL_SATURATION:
					return VkBlendOp::VK_BLEND_OP_HSL_SATURATION_EXT;

				case Flint::ColorBlendOperator::HSL_COLOR:
					return VkBlendOp::VK_BLEND_OP_HSL_COLOR_EXT;

				case Flint::ColorBlendOperator::HSL_LUMINOSITY:
					return VkBlendOp::VK_BLEND_OP_HSL_LUMINOSITY_EXT;

				case Flint::ColorBlendOperator::PLUS:
					return VkBlendOp::VK_BLEND_OP_PLUS_EXT;

				case Flint::ColorBlendOperator::PLUS_CLAMPED:
					return VkBlendOp::VK_BLEND_OP_PLUS_CLAMPED_EXT;

				case Flint::ColorBlendOperator::PLUS_CLAMPED_ALPHA:
					return VkBlendOp::VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT;

				case Flint::ColorBlendOperator::PLUS_DARKER:
					return VkBlendOp::VK_BLEND_OP_PLUS_DARKER_EXT;

				case Flint::ColorBlendOperator::MINUS:
					return VkBlendOp::VK_BLEND_OP_MINUS_EXT;

				case Flint::ColorBlendOperator::MINUS_CLAMPED:
					return VkBlendOp::VK_BLEND_OP_MINUS_CLAMPED_EXT;

				case Flint::ColorBlendOperator::CONTRAST:
					return VkBlendOp::VK_BLEND_OP_CONTRAST_EXT;

				case Flint::ColorBlendOperator::INVERT_OVG:
					return VkBlendOp::VK_BLEND_OP_INVERT_OVG_EXT;

				case Flint::ColorBlendOperator::RED:
					return VkBlendOp::VK_BLEND_OP_RED_EXT;

				case Flint::ColorBlendOperator::GREEN:
					return VkBlendOp::VK_BLEND_OP_GREEN_EXT;

				case Flint::ColorBlendOperator::BLUE:
					return VkBlendOp::VK_BLEND_OP_BLUE_EXT;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid color blend operator!");
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

			VkSampleCountFlags GetSampleCount(RasterizationSamples samples)
			{
				VkSampleCountFlags flags = 0;

				if ((samples & RasterizationSamples::BITS_1) == RasterizationSamples::BITS_1)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

				if ((samples & RasterizationSamples::BITS_2) == RasterizationSamples::BITS_2)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;

				if ((samples & RasterizationSamples::BITS_4) == RasterizationSamples::BITS_4)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;

				if ((samples & RasterizationSamples::BITS_8) == RasterizationSamples::BITS_8)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;

				if ((samples & RasterizationSamples::BITS_16) == RasterizationSamples::BITS_16)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;

				if ((samples & RasterizationSamples::BITS_32) == RasterizationSamples::BITS_32)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;

				if ((samples & RasterizationSamples::BITS_64) == RasterizationSamples::BITS_64)
					flags |= VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;

				return flags;
			}

			void AddResourceBindingsToVector(std::vector<VkDescriptorSetLayoutBinding>& bindings, const VulkanShader& shader)
			{
				std::vector<VkDescriptorSetLayoutBinding> tempBindings = shader.GetResourceBindings();
				bindings.insert(bindings.end(), tempBindings.begin(), tempBindings.end());
			}

			void AddPushConstantRangesToVector(std::vector<VkPushConstantRange>& ranges, const VulkanShader& shader)
			{
				std::vector<VkPushConstantRange> tempRanges = shader.GetPushConstantRanges();
				ranges.insert(ranges.end(), tempRanges.begin(), tempRanges.end());
			}

			void AddPoolSizesToVector(std::vector<VkDescriptorPoolSize>& poolSizes, const VulkanShader& shader)
			{
				std::vector<VkDescriptorPoolSize> tempPoolSizes = shader.GetPoolSizes();
				poolSizes.insert(poolSizes.end(), tempPoolSizes.begin(), tempPoolSizes.end());
			}

			void AddResourcesToMap(std::unordered_map<std::string, ShaderResource>& resources, const VulkanShader& shader)
			{
				const std::unordered_map<std::string, ShaderResource> tempResources = shader.GetShaderResources();
				resources.insert(tempResources.begin(), tempResources.end());
			}
		}

		VulkanGraphicsPipeline::VulkanGraphicsPipeline(
			const std::shared_ptr<Device>& pDevice,
			const std::string& pipelineName,
			const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
			const std::shared_ptr<Shader>& pVertexShader,
			const std::shared_ptr<Shader>& pTessellationControlShader,
			const std::shared_ptr<Shader>& pTessellationEvaluationShader,
			const std::shared_ptr<Shader>& pGeometryShader,
			const std::shared_ptr<Shader>& pFragmentShader,
			const GraphicsPipelineSpecification& specification)
			: GraphicsPipeline(
				pDevice,
				pipelineName,
				pScreenBoundRenderTarget,
				pVertexShader,
				pTessellationControlShader,
				pTessellationEvaluationShader,
				pGeometryShader,
				pFragmentShader,
				specification)
		{
			FLINT_SETUP_PROFILER();

			vRenderPass = pScreenBoundRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>().GetRenderPass();

			SetupDefaults();
			CreatePipelineLayout();
			CreatePipelineCache();
			CreatePipeline();
		}

		VulkanGraphicsPipeline::VulkanGraphicsPipeline(
			const std::shared_ptr<Device>& pDevice,
			const std::string& pipelineName,
			const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget,
			const std::shared_ptr<Shader>& pVertexShader,
			const std::shared_ptr<Shader>& pTessellationControlShader,
			const std::shared_ptr<Shader>& pTessellationEvaluationShader,
			const std::shared_ptr<Shader>& pGeometryShader,
			const std::shared_ptr<Shader>& pFragmentShader,
			const GraphicsPipelineSpecification& specification)
			: GraphicsPipeline(
				pDevice,
				pipelineName,
				pOffScreenRenderTarget,
				pVertexShader,
				pTessellationControlShader,
				pTessellationEvaluationShader,
				pGeometryShader,
				pFragmentShader,
				specification)
		{
			FLINT_SETUP_PROFILER();

			vRenderPass = pOffScreenRenderTarget->StaticCast<VulkanOffScreenRenderTarget>().GetRenderPass();

			SetupDefaults();
			CreatePipelineLayout();
			CreatePipelineCache();
			CreatePipeline();
		}

		void VulkanGraphicsPipeline::Recreate(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			vRenderPass = pScreenBoundRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>().GetRenderPass();

			vkDestroyPipeline(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vPipeline, nullptr);
			CreatePipeline();
		}

		void VulkanGraphicsPipeline::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			// Write cache data.
			UI64 cacheSize = 0;
			FLINT_VK_ASSERT(vkGetPipelineCacheData(vDevice.GetLogicalDevice(), vPipelineCache, &cacheSize, nullptr));

			unsigned char* pDataStore = new unsigned char[cacheSize];
			FLINT_VK_ASSERT(vkGetPipelineCacheData(vDevice.GetLogicalDevice(), vPipelineCache, &cacheSize, pDataStore));

			WriteDataToCacheFile(cacheSize, pDataStore);
			delete[] pDataStore;

			// Delete pipeline objects.
			vkDestroyPipeline(vDevice.GetLogicalDevice(), vPipeline, nullptr);
			vkDestroyDescriptorSetLayout(vDevice.GetLogicalDevice(), vDescriptorSetLayout, nullptr);
			vkDestroyPipelineLayout(vDevice.GetLogicalDevice(), vPipelineLayout, nullptr);
			vkDestroyPipelineCache(vDevice.GetLogicalDevice(), vPipelineCache, nullptr);

			if (vDescriptorSetPool)
				vkDestroyDescriptorPool(vDevice.GetLogicalDevice(), vDescriptorSetPool, nullptr);
		}

		void VulkanGraphicsPipeline::PrepareResourcesToDraw()
		{
			FLINT_SETUP_PROFILER();
			UI32 descriptorSetCount = static_cast<UI32>(mDrawDataList.size());

			// Check if should prepare resources, and return if not.
			if (!bShouldPrepareResources || mDrawDataList.empty())
				return;

			if (vDescriptorSetPool)
				vkDestroyDescriptorPool(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vDescriptorSetPool, nullptr);

			std::vector<VkDescriptorPoolSize> vPoolSizes;
			std::unordered_map<std::string, ShaderResource> resources;

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			// Resolve vertex shader data.
			{
				VulkanShader& vVertexShader = pVertexShader->StaticCast<VulkanShader>();
				_Helpers::AddPoolSizesToVector(vPoolSizes, vVertexShader);
				_Helpers::AddResourcesToMap(resources, vVertexShader);
			}

			// Check and resolve fragment shader data.
			if (pFragmentShader)
			{
				VulkanShader& vFragmentShader = pFragmentShader->StaticCast<VulkanShader>();
				_Helpers::AddPoolSizesToVector(vPoolSizes, vFragmentShader);
				_Helpers::AddResourcesToMap(resources, vFragmentShader);
			}

			// Check and resolve tessellation control shader data.
			if (pTessellationControlShader)
			{
				VulkanShader& vShader = pTessellationControlShader->StaticCast<VulkanShader>();
				_Helpers::AddPoolSizesToVector(vPoolSizes, vShader);
				_Helpers::AddResourcesToMap(resources, vShader);
			}

			// Check and resolve tessellation evaluation shader data.
			if (pTessellationEvaluationShader)
			{
				VulkanShader& vShader = pTessellationEvaluationShader->StaticCast<VulkanShader>();
				_Helpers::AddPoolSizesToVector(vPoolSizes, vShader);
				_Helpers::AddResourcesToMap(resources, vShader);
			}

			// Check and resolve geometry shader data.
			if (pGeometryShader)
			{
				VulkanShader& vShader = pGeometryShader->StaticCast<VulkanShader>();
				_Helpers::AddPoolSizesToVector(vPoolSizes, vShader);
				_Helpers::AddResourcesToMap(resources, vShader);
			}

			// Return if the pool size is empty.
			if (vPoolSizes.empty())
				return;

			// Create descriptor pool.
			{
				VkDescriptorPoolCreateInfo vPoolCreateInfo = {};
				vPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				vPoolCreateInfo.pNext = VK_NULL_HANDLE;
				vPoolCreateInfo.flags = 0;
				vPoolCreateInfo.maxSets = descriptorSetCount;
				vPoolCreateInfo.poolSizeCount = static_cast<UI32>(vPoolSizes.size());
				vPoolCreateInfo.pPoolSizes = vPoolSizes.data();

				FLINT_VK_ASSERT(vkCreateDescriptorPool(vDevice.GetLogicalDevice(), &vPoolCreateInfo, nullptr, &vDescriptorSetPool));
			}

			// Allocate descriptor sets.
			std::vector<VkDescriptorSet> vDescriptorSets(descriptorSetCount);
			{
				std::vector<VkDescriptorSetLayout> vDescriptorSetLayouts(descriptorSetCount, vDescriptorSetLayout);

				VkDescriptorSetAllocateInfo vAllocateInfo = {};
				vAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				vAllocateInfo.pNext = VK_NULL_HANDLE;
				vAllocateInfo.descriptorPool = vDescriptorSetPool;
				vAllocateInfo.descriptorSetCount = descriptorSetCount;
				vAllocateInfo.pSetLayouts = vDescriptorSetLayouts.data();

				FLINT_VK_ASSERT(vkAllocateDescriptorSets(vDevice.GetLogicalDevice(), &vAllocateInfo, vDescriptorSets.data()));
			}

			// Update descriptor sets.
			std::vector<VkWriteDescriptorSet> vWrites;
			VkWriteDescriptorSet vWrite = {};
			vWrite.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			vWrite.pNext = VK_NULL_HANDLE;
			vWrite.pTexelBufferView = VK_NULL_HANDLE;
			vWrite.descriptorCount = 1;
			vWrite.dstArrayElement = 0;

			UI64 descriptorIndex = 0;
			for (const auto drawData : mDrawDataList)
			{
				if (!drawData.second.pResourceMap)
					continue;

				vWrite.dstSet = vDescriptorSets[descriptorIndex++];
				vDescriptorSetMap[drawData.second.pResourceMap] = vWrite.dstSet;

				vWrite.pImageInfo = VK_NULL_HANDLE;

				// Get buffer resources.
				{
					const auto bufferResources = drawData.second.pResourceMap->GetBufferResourceMap();
					for (const auto resource : bufferResources)
					{
						const ShaderResource sResource = resources[resource.first];
						vWrite.descriptorType = Utilities::GetDescriptorType(sResource.mType);
						vWrite.dstBinding = sResource.mBinding;

						VkDescriptorBufferInfo* pBufferInfo = new VkDescriptorBufferInfo;
						pBufferInfo->buffer = resource.second->StaticCast<VulkanBuffer>().GetBuffer();
						pBufferInfo->range = resource.second->GetSize();
						pBufferInfo->offset = 0;

						vWrite.pBufferInfo = pBufferInfo;
						INSERT_INTO_VECTOR(vWrites, vWrite);
					}
				}

				vWrite.pBufferInfo = VK_NULL_HANDLE;

				// Get image resources.
				{
					const auto imageResources = drawData.second.pResourceMap->GetImageResourceMap();
					for (const auto resource : imageResources)
					{
						const ShaderResource sResource = resources[resource.first];
						vWrite.descriptorType = Utilities::GetDescriptorType(sResource.mType);
						vWrite.dstBinding = sResource.mBinding;

						VulkanImage& vImage = resource.second.second->StaticCast<VulkanImage>();
						VkDescriptorImageInfo* pImageInfo = new VkDescriptorImageInfo;
						pImageInfo->imageLayout = vImage.GetImageLayout();
						pImageInfo->imageView = vImage.GetImageView(0);
						pImageInfo->sampler = resource.second.first->StaticCast<VulkanImageSampler>().GetSampler();

						vWrite.pImageInfo = pImageInfo;
						INSERT_INTO_VECTOR(vWrites, vWrite);
					}
				}
			}

			vkUpdateDescriptorSets(vDevice.GetLogicalDevice(), static_cast<UI32>(vWrites.size()), vWrites.data(), 0, nullptr);

			// Clear memory.
			for (auto write : vWrites)
			{
				if (write.pBufferInfo)
					delete write.pBufferInfo;

				else if (write.pImageInfo)
					delete write.pImageInfo;
			}

			bShouldPrepareResources = false;
		}

		const VkDescriptorSet VulkanGraphicsPipeline::GetDescriptorSet(const std::shared_ptr<ResourceMap>& pResourceMap) const
		{
			if (vDescriptorSetMap.find(pResourceMap) == vDescriptorSetMap.end())
				return VK_NULL_HANDLE;

			return vDescriptorSetMap.at(pResourceMap);
		}

		const VkDescriptorSet* VulkanGraphicsPipeline::GetDescriptorSetAddress(const std::shared_ptr<ResourceMap>& pResourceMap) const
		{
			if (vDescriptorSetMap.find(pResourceMap) == vDescriptorSetMap.end())
				return nullptr;

			return &vDescriptorSetMap.at(pResourceMap);
		}

		void VulkanGraphicsPipeline::SetupDefaults()
		{
			// Resolve shader stages.
			INSERT_INTO_VECTOR(vShaderStageCreateInfo, pVertexShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pFragmentShader)
				INSERT_INTO_VECTOR(vShaderStageCreateInfo, pFragmentShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pTessellationControlShader)
				INSERT_INTO_VECTOR(vShaderStageCreateInfo, pTessellationControlShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pTessellationEvaluationShader)
				INSERT_INTO_VECTOR(vShaderStageCreateInfo, pTessellationEvaluationShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pGeometryShader)
				INSERT_INTO_VECTOR(vShaderStageCreateInfo, pGeometryShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			// Resolve vertex input state.
			{
				VkVertexInputAttributeDescription vAttributeDescription = {};
				VkVertexInputBindingDescription vBindingDescription = {};
				const auto inputAttributes = pVertexShader->GetInputAttributes();

				for (const auto binding : inputAttributes)
				{
					vAttributeDescription.binding = binding.first;
					vAttributeDescription.offset = 0;
					for (const auto attribute : binding.second)
					{
						vAttributeDescription.location = attribute.mLocation;
						vAttributeDescription.format = _Helpers::GetFormatFromSize(attribute.mDataType);

						INSERT_INTO_VECTOR(vVertexAttributes, vAttributeDescription);
						vAttributeDescription.offset += static_cast<UI32>(attribute.mDataType);
					}

					vBindingDescription.binding = binding.first;
					vBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
					vBindingDescription.stride = vAttributeDescription.offset;
					INSERT_INTO_VECTOR(vVertexBindings, vBindingDescription);
				}
			}

			vVertexInputStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vVertexInputStateCreateInfo.pNext = VK_NULL_HANDLE;
			vVertexInputStateCreateInfo.flags = 0;
			vVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<UI32>(vVertexAttributes.size());
			vVertexInputStateCreateInfo.pVertexAttributeDescriptions = vVertexAttributes.data();
			vVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<UI32>(vVertexBindings.size());
			vVertexInputStateCreateInfo.pVertexBindingDescriptions = vVertexBindings.data();

			// Input assembly state.
			vInputAssemblyStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			vInputAssemblyStateCreateInfo.pNext = VK_NULL_HANDLE;
			vInputAssemblyStateCreateInfo.flags = 0;
			vInputAssemblyStateCreateInfo.primitiveRestartEnable = GET_VK_BOOL(mSpecification.bEnablePrimitiveRestart);
			vInputAssemblyStateCreateInfo.topology = _Helpers::GetPrimitiveTopology(mSpecification.mPrimitiveTopology);

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
				vAttachmentState.alphaBlendOp = _Helpers::GetBlendOp(attachment.mAlphaBlendOperator);
				vAttachmentState.colorBlendOp = _Helpers::GetBlendOp(attachment.mBlendOperator);
				vAttachmentState.colorWriteMask = _Helpers::GetComponentFlags(attachment.mColorWriteMask);
				vAttachmentState.srcColorBlendFactor = _Helpers::GetBlendFactor(attachment.mSrcBlendFactor);
				vAttachmentState.srcAlphaBlendFactor = _Helpers::GetBlendFactor(attachment.mSrcAlphaBlendFactor);
				vAttachmentState.dstAlphaBlendFactor = _Helpers::GetBlendFactor(attachment.mDstAlphaBlendFactor);
				vAttachmentState.dstColorBlendFactor = _Helpers::GetBlendFactor(attachment.mDstBlendFactor);

				INSERT_INTO_VECTOR(vCBASS, vAttachmentState);
			}

			vColorBlendStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			vColorBlendStateCreateInfo.pNext = VK_NULL_HANDLE;
			vColorBlendStateCreateInfo.flags = 0;
			vColorBlendStateCreateInfo.logicOp = _Helpers::GetLogicOp(mSpecification.mColorBlendLogic);
			vColorBlendStateCreateInfo.logicOpEnable = GET_VK_BOOL(mSpecification.bEnableColorBlendLogic);
			std::copy(mSpecification.mColorBlendConstants, mSpecification.mColorBlendConstants + 4, vColorBlendStateCreateInfo.blendConstants);

			vColorBlendStateCreateInfo.attachmentCount = static_cast<UI32>(vCBASS.size());
			vColorBlendStateCreateInfo.pAttachments = vCBASS.data();

			// Rasterization state.
			vRasterizationStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			vRasterizationStateCreateInfo.pNext = VK_NULL_HANDLE;
			vRasterizationStateCreateInfo.flags = 0;
			vRasterizationStateCreateInfo.cullMode = _Helpers::GetCullMode(mSpecification.mCullMode);
			vRasterizationStateCreateInfo.depthBiasEnable = GET_VK_BOOL(mSpecification.bEnableDepthBias);
			vRasterizationStateCreateInfo.depthBiasClamp = mSpecification.mDepthBiasFactor;
			vRasterizationStateCreateInfo.depthBiasConstantFactor = mSpecification.mDepthConstantFactor;
			vRasterizationStateCreateInfo.depthBiasSlopeFactor = mSpecification.mDepthSlopeFactor;
			vRasterizationStateCreateInfo.depthClampEnable = GET_VK_BOOL(mSpecification.bEnableDepthClamp);
			vRasterizationStateCreateInfo.frontFace = _Helpers::GetFrontFace(mSpecification.mFrontFace);
			vRasterizationStateCreateInfo.lineWidth = mSpecification.mRasterizerLineWidth;
			vRasterizationStateCreateInfo.polygonMode = _Helpers::GetPolygonMode(mSpecification.mPolygonMode);
			vRasterizationStateCreateInfo.rasterizerDiscardEnable = GET_VK_BOOL(mSpecification.bEnableRasterizerDiscard);

			// Multisample state.
			vMultisampleStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			vMultisampleStateCreateInfo.pNext = VK_NULL_HANDLE;
			vMultisampleStateCreateInfo.flags = 0;
			vMultisampleStateCreateInfo.alphaToCoverageEnable = GET_VK_BOOL(mSpecification.bEnableAlphaCoverage);
			vMultisampleStateCreateInfo.alphaToOneEnable = GET_VK_BOOL(mSpecification.bEnableAlphaToOne);
			vMultisampleStateCreateInfo.minSampleShading = mSpecification.mMinSampleShading;
			vMultisampleStateCreateInfo.pSampleMask;	// TODO
			//vMultisampleStateCreateInfo.rasterizationSamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
			vMultisampleStateCreateInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(_Helpers::GetSampleCount(mSpecification.mRasterizationSamples));
			//vMultisampleStateCreateInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(pDevice->StaticCast<VulkanDevice>().GetSampleCount());
			vMultisampleStateCreateInfo.sampleShadingEnable = GET_VK_BOOL(mSpecification.bEnableSampleShading);

			// Depth stencil state.
			vDepthStencilStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			vDepthStencilStateCreateInfo.pNext = VK_NULL_HANDLE;
			vDepthStencilStateCreateInfo.flags = 0;
			vDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			vDepthStencilStateCreateInfo.depthTestEnable = GET_VK_BOOL(mSpecification.bEnableDepthTest);
			vDepthStencilStateCreateInfo.depthWriteEnable = GET_VK_BOOL(mSpecification.bEnableDepthWrite);
			vDepthStencilStateCreateInfo.depthCompareOp = _Helpers::GetCompareOp(mSpecification.mDepthCompareLogic);

			// Dynamic state.
			vDynamicStates = std::move(_Helpers::GetDynamicStates(mSpecification.mDynamicStateFlags));

			vDynamicStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			vDynamicStateCreateInfo.pNext = VK_NULL_HANDLE;
			vDynamicStateCreateInfo.flags = 0;
			vDynamicStateCreateInfo.dynamicStateCount = static_cast<UI32>(vDynamicStates.size());
			vDynamicStateCreateInfo.pDynamicStates = vDynamicStates.data();
		}

		void VulkanGraphicsPipeline::CreatePipelineCache()
		{
			FLINT_SETUP_PROFILER();

			auto [size, data] = ReadDataFromCacheFile();

			VkPipelineCacheCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.initialDataSize = size;
			vCreateInfo.pInitialData = data;

			FLINT_VK_ASSERT(vkCreatePipelineCache(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCreateInfo, nullptr, &vPipelineCache));

			delete[] data;
		}

		void VulkanGraphicsPipeline::CreatePipelineLayout()
		{
			FLINT_SETUP_PROFILER();

			std::vector<VkDescriptorSetLayoutBinding> vBindings;
			std::vector<VkPushConstantRange> vConstantRanges;

			// Resolve vertex shader data.
			_Helpers::AddResourceBindingsToVector(vBindings, pVertexShader->StaticCast<VulkanShader>());
			_Helpers::AddPushConstantRangesToVector(vConstantRanges, pVertexShader->StaticCast<VulkanShader>());

			// Check and resolve fragment shader data.
			if (pFragmentShader)
			{
				_Helpers::AddResourceBindingsToVector(vBindings, pFragmentShader->StaticCast<VulkanShader>());
				_Helpers::AddPushConstantRangesToVector(vConstantRanges, pFragmentShader->StaticCast<VulkanShader>());
			}

			// Check and resolve tessellation control shader data.
			if (pTessellationControlShader)
			{
				_Helpers::AddResourceBindingsToVector(vBindings, pTessellationControlShader->StaticCast<VulkanShader>());
				_Helpers::AddPushConstantRangesToVector(vConstantRanges, pTessellationControlShader->StaticCast<VulkanShader>());
			}

			// Check and resolve tessellation evaluation shader data.
			if (pTessellationEvaluationShader)
			{
				_Helpers::AddResourceBindingsToVector(vBindings, pTessellationEvaluationShader->StaticCast<VulkanShader>());
				_Helpers::AddPushConstantRangesToVector(vConstantRanges, pTessellationEvaluationShader->StaticCast<VulkanShader>());
			}

			// Check and resolve geometry shader data.
			if (pGeometryShader)
			{
				_Helpers::AddResourceBindingsToVector(vBindings, pGeometryShader->StaticCast<VulkanShader>());
				_Helpers::AddPushConstantRangesToVector(vConstantRanges, pGeometryShader->StaticCast<VulkanShader>());
			}

			// Create descriptor set layout.
			{
				VkDescriptorSetLayoutCreateInfo vLayoutCreateInfo = {};
				vLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				vLayoutCreateInfo.pNext = VK_NULL_HANDLE;
				vLayoutCreateInfo.flags = 0;
				vLayoutCreateInfo.bindingCount = static_cast<UI32>(vBindings.size());
				vLayoutCreateInfo.pBindings = vBindings.data();

				FLINT_VK_ASSERT(vkCreateDescriptorSetLayout(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vLayoutCreateInfo, nullptr, &vDescriptorSetLayout));
			}

			// Create pipeline layout.
			VkPipelineLayoutCreateInfo vCI = {};
			vCI.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.flags = 0;
			vCI.pushConstantRangeCount = static_cast<UI32>(vConstantRanges.size());
			vCI.pPushConstantRanges = vConstantRanges.data();
			vCI.setLayoutCount = 1;
			vCI.pSetLayouts = &vDescriptorSetLayout;

			FLINT_VK_ASSERT(vkCreatePipelineLayout(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCI, nullptr, &vPipelineLayout));
		}

		void VulkanGraphicsPipeline::CreatePipeline()
		{
			FLINT_SETUP_PROFILER();

			// Resolve viewport state.
			VkRect2D vR2D = {};
			vR2D.extent.width = static_cast<UI32>(pRenderTarget->GetExtent().mWidth);
			vR2D.extent.height = static_cast<UI32>(pRenderTarget->GetExtent().mHeight);
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
			vCreateInfo.stageCount = static_cast<UI32>(vShaderStageCreateInfo.size());
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

			FLINT_VK_ASSERT(vkCreateGraphicsPipelines(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vPipelineCache, 1, &vCreateInfo, nullptr, &vPipeline));
		}
	}
}