// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRasterizingPipeline.hpp"
#include "VulkanBackend/VulkanRasterizer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

#ifdef FLINT_PLATFORM_WINDOWS
#	include <execution>

#endif

namespace /* anonymous */
{
	/**
	 * Get the primitive topology.
	 *
	 * @param topology The flint primitive topology.
	 * @return The Vulkan primitive topology.
	 */
	VkPrimitiveTopology GetPrimitiveTopology(Flint::Core::PrimitiveTopology topology)
	{
		switch (topology)
		{
		case Flint::Core::PrimitiveTopology::PointList:						return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case Flint::Core::PrimitiveTopology::LineList:						return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case Flint::Core::PrimitiveTopology::LineStrip:						return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case Flint::Core::PrimitiveTopology::TriangleList:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case Flint::Core::PrimitiveTopology::TriangleStrip:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case Flint::Core::PrimitiveTopology::TriangleFan:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		case Flint::Core::PrimitiveTopology::LineListWithAdjacency:			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
		case Flint::Core::PrimitiveTopology::LineStripWithAdjacency:		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
		case Flint::Core::PrimitiveTopology::TriangleListWithAdjacency:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
		case Flint::Core::PrimitiveTopology::TriangleStripWithAdjacency:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
		case Flint::Core::PrimitiveTopology::PatchList:						return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		default:															throw Flint::BackendError("Invalid or Undefined primitive topology!");
		}
	}

	/**
	 * Get the cull mode.
	 *
	 * @param cull The flint cull mode.
	 * @return The Vulkan cull mode.
	 */
	VkCullModeFlags GetCullMode(Flint::Core::CullMode cull)
	{
		switch (cull)
		{
		case Flint::Core::CullMode::None:									return VK_CULL_MODE_NONE;
		case Flint::Core::CullMode::Front:									return VK_CULL_MODE_FRONT_BIT;
		case Flint::Core::CullMode::Back:									return VK_CULL_MODE_BACK_BIT;
		case Flint::Core::CullMode::FrontAndBack:							return VK_CULL_MODE_FRONT_AND_BACK;
		default:															throw Flint::BackendError("Invalid or Undefined cull mode!");
		}
	}

	/**
	 * Get the front face.
	 *
	 * @param face The flint front face.
	 * @return The Vulkan front face.
	 */
	VkFrontFace GetFrontFace(Flint::Core::FrontFace face)
	{
		switch (face)
		{
		case Flint::Core::FrontFace::CounterClockwise:						return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case Flint::Core::FrontFace::Clockwise:								return VK_FRONT_FACE_CLOCKWISE;
		default:															throw Flint::BackendError("Invalid or Undefined front face!");
		}
	}

	/**
	 * Get the polygon mode.
	 *
	 * @param mode The flint polygon mode.
	 * @return The Vulkan polygon mode.
	 */
	VkPolygonMode GetPolygonMode(Flint::Core::PolygonMode mode)
	{
		switch (mode)
		{
		case Flint::Core::PolygonMode::Fill:								return VK_POLYGON_MODE_FILL;
		case Flint::Core::PolygonMode::Line:								return VK_POLYGON_MODE_LINE;
		case Flint::Core::PolygonMode::Point:								return VK_POLYGON_MODE_POINT;
		default:															throw Flint::BackendError("Invalid or Undefined polygon mode!");
		}
	}

	/**
	 * Get the logic operator.
	 *
	 * @param logic The flint logic.
	 * @return The Vulkan logic.
	 */
	VkLogicOp GetLogicOp(Flint::Core::ColorBlendLogic logic)
	{
		switch (logic)
		{
		case Flint::Core::ColorBlendLogic::Clear:							return VK_LOGIC_OP_CLEAR;
		case Flint::Core::ColorBlendLogic::And:								return VK_LOGIC_OP_AND;
		case Flint::Core::ColorBlendLogic::AndReverse:						return VK_LOGIC_OP_AND_REVERSE;
		case Flint::Core::ColorBlendLogic::Copy:							return VK_LOGIC_OP_COPY;
		case Flint::Core::ColorBlendLogic::AndInverted:						return VK_LOGIC_OP_AND_INVERTED;
		case Flint::Core::ColorBlendLogic::NoOperator:						return VK_LOGIC_OP_NO_OP;
		case Flint::Core::ColorBlendLogic::XOR:								return VK_LOGIC_OP_XOR;
		case Flint::Core::ColorBlendLogic::OR:								return VK_LOGIC_OP_OR;
		case Flint::Core::ColorBlendLogic::NOR:								return VK_LOGIC_OP_NOR;
		case Flint::Core::ColorBlendLogic::Equivalent:						return VK_LOGIC_OP_EQUIVALENT;
		case Flint::Core::ColorBlendLogic::Invert:							return VK_LOGIC_OP_INVERT;
		case Flint::Core::ColorBlendLogic::ReverseOR:						return VK_LOGIC_OP_OR_REVERSE;
		case Flint::Core::ColorBlendLogic::CopyInverted:					return VK_LOGIC_OP_COPY_INVERTED;
		case Flint::Core::ColorBlendLogic::InvertedOR:						return VK_LOGIC_OP_OR_INVERTED;
		case Flint::Core::ColorBlendLogic::NAND:							return VK_LOGIC_OP_NAND;
		case Flint::Core::ColorBlendLogic::Set:								return VK_LOGIC_OP_SET;
		default:															throw Flint::BackendError("Invalid or Undefined color blend logic!");
		}
	}

	/**
	 * Get the compare operator.
	 *
	 * @param logic the flint logic.
	 * @return The Vulkan logic operator.
	 */
	VkCompareOp GetCompareOp(Flint::Core::DepthCompareLogic logic)
	{
		switch (logic)
		{
		case Flint::Core::DepthCompareLogic::Never:							return VK_COMPARE_OP_NEVER;
		case Flint::Core::DepthCompareLogic::Less:							return VK_COMPARE_OP_LESS;
		case Flint::Core::DepthCompareLogic::Equal:							return VK_COMPARE_OP_EQUAL;
		case Flint::Core::DepthCompareLogic::LessOrEqual:					return VK_COMPARE_OP_LESS_OR_EQUAL;
		case Flint::Core::DepthCompareLogic::Greater:						return VK_COMPARE_OP_GREATER;
		case Flint::Core::DepthCompareLogic::NotEqual:						return VK_COMPARE_OP_NOT_EQUAL;
		case Flint::Core::DepthCompareLogic::GreaterOrEqual:				return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case Flint::Core::DepthCompareLogic::Always:						return VK_COMPARE_OP_ALWAYS;
		default:															throw Flint::BackendError("Invalid or Undefined depth compare logic!");
		}
	}

	/**
	 * Get the dynamic states.
	 *
	 * @param flags The flint flags.
	 * @return The Vulkan flags.
	 */
	std::vector<VkDynamicState> GetDynamicStates(Flint::Core::DynamicStateFlags flags)
	{
		std::vector<VkDynamicState> states;
		if ((flags & Flint::Core::DynamicStateFlags::ViewPort) == Flint::Core::DynamicStateFlags::ViewPort) states.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		if ((flags & Flint::Core::DynamicStateFlags::Scissor) == Flint::Core::DynamicStateFlags::Scissor) states.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
		if ((flags & Flint::Core::DynamicStateFlags::LineWidth) == Flint::Core::DynamicStateFlags::LineWidth) states.emplace_back(VK_DYNAMIC_STATE_LINE_WIDTH);
		if ((flags & Flint::Core::DynamicStateFlags::DepthBias) == Flint::Core::DynamicStateFlags::DepthBias) states.emplace_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
		if ((flags & Flint::Core::DynamicStateFlags::BlendConstants) == Flint::Core::DynamicStateFlags::BlendConstants) states.emplace_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
		if ((flags & Flint::Core::DynamicStateFlags::DepthBounds) == Flint::Core::DynamicStateFlags::DepthBounds) states.emplace_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);

		return states;
	}

	/**
	 * Get the blend factor.
	 *
	 * @param factor The flint factor.
	 * @return The Vulkan factor.
	 */
	VkBlendFactor GetBlendFactor(Flint::Core::ColorBlendFactor factor)
	{
		switch (factor)
		{
		case Flint::Core::ColorBlendFactor::Zero:							return VK_BLEND_FACTOR_ZERO;
		case Flint::Core::ColorBlendFactor::One:							return VK_BLEND_FACTOR_ONE;
		case Flint::Core::ColorBlendFactor::SourceColor:					return VK_BLEND_FACTOR_SRC_COLOR;
		case Flint::Core::ColorBlendFactor::OneMinusSourceColor:			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case Flint::Core::ColorBlendFactor::DestinationColor:				return VK_BLEND_FACTOR_DST_COLOR;
		case Flint::Core::ColorBlendFactor::OneMinusDestinationColor:		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case Flint::Core::ColorBlendFactor::SourceAlpha:					return VK_BLEND_FACTOR_SRC_ALPHA;
		case Flint::Core::ColorBlendFactor::OneMinusSourceAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case Flint::Core::ColorBlendFactor::DestinationAlpha:				return VK_BLEND_FACTOR_DST_ALPHA;
		case Flint::Core::ColorBlendFactor::OneMinusDestinationAlpha:		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case Flint::Core::ColorBlendFactor::ConstantColor:					return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case Flint::Core::ColorBlendFactor::OneMinusConstantColor:			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		case Flint::Core::ColorBlendFactor::ConstantAlpha:					return VK_BLEND_FACTOR_CONSTANT_ALPHA;
		case Flint::Core::ColorBlendFactor::OneMinusConstantAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
		case Flint::Core::ColorBlendFactor::SourceAlphaSaturate:			return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		case Flint::Core::ColorBlendFactor::SourceOneColor:					return VK_BLEND_FACTOR_SRC1_COLOR;
		case Flint::Core::ColorBlendFactor::OneMinusSourceOneColor:			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
		case Flint::Core::ColorBlendFactor::SourceOneAlpha:					return VK_BLEND_FACTOR_SRC1_ALPHA;
		case Flint::Core::ColorBlendFactor::OneMinusSourceOneAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
		default:															throw Flint::BackendError("Invalid color blend factor!");
		}
	}

	/**
	 * Get the blend operator.
	 *
	 * @param oprtr The flint operator.
	 * @return The Vulkan blend operator.
	 */
	VkBlendOp GetBlendOp(Flint::Core::ColorBlendOperator oprtr)
	{
		switch (oprtr)
		{
		case Flint::Core::ColorBlendOperator::Add:							return VK_BLEND_OP_ADD;
		case Flint::Core::ColorBlendOperator::Subtract:						return VK_BLEND_OP_SUBTRACT;
		case Flint::Core::ColorBlendOperator::ReverseSubtract:				return VK_BLEND_OP_REVERSE_SUBTRACT;
		case Flint::Core::ColorBlendOperator::Minimum:						return VK_BLEND_OP_MIN;
		case Flint::Core::ColorBlendOperator::Maximum:						return VK_BLEND_OP_MAX;
		case Flint::Core::ColorBlendOperator::Zero:							return VK_BLEND_OP_ZERO_EXT;
		case Flint::Core::ColorBlendOperator::Source:						return VK_BLEND_OP_SRC_EXT;
		case Flint::Core::ColorBlendOperator::Destination:					return VK_BLEND_OP_DST_EXT;
		case Flint::Core::ColorBlendOperator::SourceOver:					return VK_BLEND_OP_SRC_OVER_EXT;
		case Flint::Core::ColorBlendOperator::DestinationOver:				return VK_BLEND_OP_DST_OVER_EXT;
		case Flint::Core::ColorBlendOperator::SourceIn:						return VK_BLEND_OP_SRC_IN_EXT;
		case Flint::Core::ColorBlendOperator::DestinationIn:				return VK_BLEND_OP_DST_IN_EXT;
		case Flint::Core::ColorBlendOperator::SouceOut:						return VK_BLEND_OP_SRC_OUT_EXT;
		case Flint::Core::ColorBlendOperator::DestinationOut:				return VK_BLEND_OP_DST_OUT_EXT;
		case Flint::Core::ColorBlendOperator::SourceATOP:					return VK_BLEND_OP_SRC_ATOP_EXT;
		case Flint::Core::ColorBlendOperator::DestinationATOP:				return VK_BLEND_OP_DST_ATOP_EXT;
		case Flint::Core::ColorBlendOperator::XOR:							return VK_BLEND_OP_XOR_EXT;
		case Flint::Core::ColorBlendOperator::Multiply:						return VK_BLEND_OP_MULTIPLY_EXT;
		case Flint::Core::ColorBlendOperator::Screen:						return VK_BLEND_OP_SCREEN_EXT;
		case Flint::Core::ColorBlendOperator::Overlay:						return VK_BLEND_OP_OVERLAY_EXT;
		case Flint::Core::ColorBlendOperator::Darken:						return VK_BLEND_OP_DARKEN_EXT;
		case Flint::Core::ColorBlendOperator::Lighten:						return VK_BLEND_OP_LIGHTEN_EXT;
		case Flint::Core::ColorBlendOperator::ColorDodge:					return VK_BLEND_OP_COLORDODGE_EXT;
		case Flint::Core::ColorBlendOperator::ColorBurn:					return VK_BLEND_OP_COLORBURN_EXT;
		case Flint::Core::ColorBlendOperator::HardLight:					return VK_BLEND_OP_HARDLIGHT_EXT;
		case Flint::Core::ColorBlendOperator::SoftLight:					return VK_BLEND_OP_SOFTLIGHT_EXT;
		case Flint::Core::ColorBlendOperator::Difference:					return VK_BLEND_OP_DIFFERENCE_EXT;
		case Flint::Core::ColorBlendOperator::Exclusion:					return VK_BLEND_OP_EXCLUSION_EXT;
		case Flint::Core::ColorBlendOperator::Invert:						return VK_BLEND_OP_INVERT_EXT;
		case Flint::Core::ColorBlendOperator::InvertRGB:					return VK_BLEND_OP_INVERT_RGB_EXT;
		case Flint::Core::ColorBlendOperator::LinearDodge:					return VK_BLEND_OP_LINEARDODGE_EXT;
		case Flint::Core::ColorBlendOperator::LinearBurn:					return VK_BLEND_OP_LINEARBURN_EXT;
		case Flint::Core::ColorBlendOperator::VividLight:					return VK_BLEND_OP_VIVIDLIGHT_EXT;
		case Flint::Core::ColorBlendOperator::LinearLight:					return VK_BLEND_OP_LINEARLIGHT_EXT;
		case Flint::Core::ColorBlendOperator::PinLight:						return VK_BLEND_OP_PINLIGHT_EXT;
		case Flint::Core::ColorBlendOperator::HardMix:						return VK_BLEND_OP_HARDMIX_EXT;
		case Flint::Core::ColorBlendOperator::HSLHue:						return VK_BLEND_OP_HSL_HUE_EXT;
		case Flint::Core::ColorBlendOperator::HSLSaturation:				return VK_BLEND_OP_HSL_SATURATION_EXT;
		case Flint::Core::ColorBlendOperator::HSLColor:						return VK_BLEND_OP_HSL_COLOR_EXT;
		case Flint::Core::ColorBlendOperator::HSLLuminosity:				return VK_BLEND_OP_HSL_LUMINOSITY_EXT;
		case Flint::Core::ColorBlendOperator::Plus:							return VK_BLEND_OP_PLUS_EXT;
		case Flint::Core::ColorBlendOperator::PlusClamped:					return VK_BLEND_OP_PLUS_CLAMPED_EXT;
		case Flint::Core::ColorBlendOperator::PlusClampedAlpha:				return VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT;
		case Flint::Core::ColorBlendOperator::PlusDarker:					return VK_BLEND_OP_PLUS_DARKER_EXT;
		case Flint::Core::ColorBlendOperator::Minus:						return VK_BLEND_OP_MINUS_EXT;
		case Flint::Core::ColorBlendOperator::MinusClamped:					return VK_BLEND_OP_MINUS_CLAMPED_EXT;
		case Flint::Core::ColorBlendOperator::Contrast:						return VK_BLEND_OP_CONTRAST_EXT;
		case Flint::Core::ColorBlendOperator::InvertOVG:					return VK_BLEND_OP_INVERT_OVG_EXT;
		case Flint::Core::ColorBlendOperator::Red:							return VK_BLEND_OP_RED_EXT;
		case Flint::Core::ColorBlendOperator::Green:						return VK_BLEND_OP_GREEN_EXT;
		case Flint::Core::ColorBlendOperator::Blue:							return VK_BLEND_OP_BLUE_EXT;
		default:															throw Flint::BackendError("Invalid color blend operator!");
		}
	}

	/**
	 * Get the color component flags.
	 *
	 * @param mask The color write mask.
	 * @return The VUlkan mask.
	 */
	VkColorComponentFlags GetComponentFlags(Flint::Core::ColorWriteMask mask)
	{
		VkColorComponentFlags flags = 0;
		if ((mask & Flint::Core::ColorWriteMask::R) == Flint::Core::ColorWriteMask::R) flags |= VK_COLOR_COMPONENT_R_BIT;
		if ((mask & Flint::Core::ColorWriteMask::G) == Flint::Core::ColorWriteMask::G) flags |= VK_COLOR_COMPONENT_G_BIT;
		if ((mask & Flint::Core::ColorWriteMask::B) == Flint::Core::ColorWriteMask::B) flags |= VK_COLOR_COMPONENT_B_BIT;
		if ((mask & Flint::Core::ColorWriteMask::A) == Flint::Core::ColorWriteMask::A) flags |= VK_COLOR_COMPONENT_A_BIT;

		return flags;
	}
}

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRasterizingPipeline::VulkanRasterizingPipeline(const std::shared_ptr<VulkanDevice>& pDevice, const std::shared_ptr<VulkanRasterizer>& pRasterizer, const Core::RasterizingPipelineSpecification& specification)
			: RasterizingPipeline(std::static_pointer_cast<Core::Device>(pDevice), std::static_pointer_cast<Core::Rasterizer>(pRasterizer), specification)
			, VulkanPipeline(pDevice, specification.m_CacheFile)
			, VulkanDescriptorSetManager(pDevice, pRasterizer->getFrameCount())
		{
			// Resolve shader information.
			std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
			std::vector<VkPushConstantRange> pushConstants;

			// Resolve the information in the vertex shader.
			//resolveShader(specification.m_VertexShader, VK_SHADER_STAGE_VERTEX_BIT, layoutBindings, pushConstants);

			// Resolve the information in the fragment shader.
			//resolveShader(specification.m_FragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, layoutBindings, pushConstants);

			// Create the descriptor set layout.
			setup(std::move(layoutBindings));

			// Create the pipeline layouts.
			createPipelineLayout(std::move(pushConstants));

			// Setup the defaults.
			setupDefaults(std::move(specification));

			// Finally create the pipeline.
			createPipeline();

			// Make sure to set the object as valid.
			validate();
		}

		VulkanRasterizingPipeline::~VulkanRasterizingPipeline()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanRasterizingPipeline::terminate()
		{
			destroyShaders();
			invalidate();
		}

		void VulkanRasterizingPipeline::recreate()
		{
			getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyPipeline(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_Pipeline, nullptr);
			createPipeline();
		}

		//void VulkanRasterizingPipeline::resolveShader(const Shader& code, VkShaderStageFlagBits stageFlag, std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, std::vector<VkPushConstantRange>& pushConstants)
		//{
		//	// Resolve the descriptors.
		//	for (const auto& binding : code.getBindings())
		//	{
		//		auto& layoutBinding = layoutBindings.emplace_back();
		//		layoutBinding.binding = binding.m_Binding;
		//		layoutBinding.descriptorCount = binding.m_Count;
		//		layoutBinding.descriptorType = Utility::GetDescriptorType(binding.m_Type);
		//		layoutBinding.pImmutableSamplers = nullptr;
		//		layoutBinding.stageFlags = stageFlag;
		//
		//		auto& poolSize = m_PoolSizes.emplace_back();
		//		poolSize.descriptorCount = layoutBinding.descriptorCount;
		//		poolSize.type = layoutBinding.descriptorType;
		//	}
		//
		//	// Resolve the push constants.
		//	for (const auto& constant : code.getPushConstants())
		//	{
		//		auto& pushConstant = pushConstants.emplace_back();
		//		pushConstant.stageFlags = stageFlag;
		//		pushConstant.size = constant.m_Size;
		//		pushConstant.offset = constant.m_Offset;
		//	}
		//
		//	// Create the shader module.
		//	VkShaderModuleCreateInfo createInfo = {};
		//	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		//	createInfo.pNext = nullptr;
		//	createInfo.flags = 0;
		//	createInfo.codeSize = code.getCode().size();
		//	createInfo.pCode = code.getCode().data();
		//
		//	VkShaderModule shaderModule = VK_NULL_HANDLE;
		//	FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreateShaderModule(getDevice().as<VulkanDevice>()->getLogicalDevice(), &createInfo, nullptr, &shaderModule), "Failed to create the shader module!");
		//
		//	// Create the shader stage info.
		//	auto& stageInfo = m_ShaderStageCreateInfo.emplace_back();
		//	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		//	stageInfo.pNext = nullptr;
		//	stageInfo.flags = 0;
		//	stageInfo.stage = stageFlag;
		//	stageInfo.module = shaderModule;
		//	stageInfo.pSpecializationInfo = nullptr;
		//	stageInfo.pName = code.getEntryPoint().data();
		//}

		void VulkanRasterizingPipeline::createPipelineLayout(std::vector<VkPushConstantRange>&& pushConstants)
		{
			VkPipelineLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
			createInfo.pPushConstantRanges = pushConstants.data();
			createInfo.setLayoutCount = 1;
			createInfo.pSetLayouts = &m_DescriptorSetLayout;

			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreatePipelineLayout(getDevice().as<VulkanDevice>()->getLogicalDevice(), &createInfo, nullptr, &m_PipelineLayout), "Failed to create the pipeline layout!");
		}

		void VulkanRasterizingPipeline::setupDefaults(const Core::RasterizingPipelineSpecification& specification)
		{
			// Setup the input bindings.
			m_VertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			m_VertexInputStateCreateInfo.pNext = nullptr;
			m_VertexInputStateCreateInfo.flags = 0;
			m_VertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexAttributes.size());
			m_VertexInputStateCreateInfo.pVertexAttributeDescriptions = m_VertexAttributes.data();
			m_VertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_VertexBindings.size());
			m_VertexInputStateCreateInfo.pVertexBindingDescriptions = m_VertexBindings.data();

			// Input assembly state.
			m_InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			m_InputAssemblyStateCreateInfo.pNext = nullptr;
			m_InputAssemblyStateCreateInfo.flags = 0;
			m_InputAssemblyStateCreateInfo.primitiveRestartEnable = GET_VK_BOOL(specification.m_EnablePrimitiveRestart);
			m_InputAssemblyStateCreateInfo.topology = GetPrimitiveTopology(specification.m_PrimitiveTopology);

			// Tessellation state.
			m_TessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			m_TessellationStateCreateInfo.pNext = nullptr;
			m_TessellationStateCreateInfo.flags = 0;
			m_TessellationStateCreateInfo.patchControlPoints = specification.m_TessellationPatchControlPoints;

			// Color blend state.
			for (const auto attachment : specification.m_ColorBlendAttachments)
			{
				VkPipelineColorBlendAttachmentState vAttachmentState = {};
				vAttachmentState.blendEnable = GET_VK_BOOL(attachment.m_EnableBlend);
				vAttachmentState.alphaBlendOp = GetBlendOp(attachment.m_AlphaBlendOperator);
				vAttachmentState.colorBlendOp = GetBlendOp(attachment.m_BlendOperator);
				vAttachmentState.colorWriteMask = GetComponentFlags(attachment.m_ColorWriteMask);
				vAttachmentState.srcColorBlendFactor = GetBlendFactor(attachment.m_SrcBlendFactor);
				vAttachmentState.srcAlphaBlendFactor = GetBlendFactor(attachment.m_SrcAlphaBlendFactor);
				vAttachmentState.dstAlphaBlendFactor = GetBlendFactor(attachment.m_DstAlphaBlendFactor);
				vAttachmentState.dstColorBlendFactor = GetBlendFactor(attachment.m_DstBlendFactor);

				m_CBASS.emplace_back(vAttachmentState);
			}

			m_ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			m_ColorBlendStateCreateInfo.pNext = nullptr;
			m_ColorBlendStateCreateInfo.flags = 0;
			m_ColorBlendStateCreateInfo.logicOp = GetLogicOp(specification.m_ColorBlendLogic);
			m_ColorBlendStateCreateInfo.logicOpEnable = GET_VK_BOOL(specification.m_EnableColorBlendLogic);

#ifdef FLINT_PLATFORM_WINDOWS
			std::copy_n(std::execution::unseq, specification.m_ColorBlendConstants, 4, m_ColorBlendStateCreateInfo.blendConstants);

#else 
			std::copy_n(specification.m_ColorBlendConstants, 4, m_ColorBlendStateCreateInfo.blendConstants);

#endif

			m_ColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(m_CBASS.size());
			m_ColorBlendStateCreateInfo.pAttachments = m_CBASS.data();

			// Rasterization state.
			m_RasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			m_RasterizationStateCreateInfo.pNext = nullptr;
			m_RasterizationStateCreateInfo.flags = 0;
			m_RasterizationStateCreateInfo.cullMode = GetCullMode(specification.m_CullMode);
			m_RasterizationStateCreateInfo.depthBiasEnable = GET_VK_BOOL(specification.m_EnableDepthBias);
			m_RasterizationStateCreateInfo.depthBiasClamp = specification.m_DepthBiasFactor;
			m_RasterizationStateCreateInfo.depthBiasConstantFactor = specification.m_DepthConstantFactor;
			m_RasterizationStateCreateInfo.depthBiasSlopeFactor = specification.m_DepthSlopeFactor;
			m_RasterizationStateCreateInfo.depthClampEnable = GET_VK_BOOL(specification.m_EnableDepthClamp);
			m_RasterizationStateCreateInfo.frontFace = GetFrontFace(specification.m_FrontFace);
			m_RasterizationStateCreateInfo.lineWidth = specification.m_RasterizerLineWidth;
			m_RasterizationStateCreateInfo.polygonMode = GetPolygonMode(specification.m_PolygonMode);
			m_RasterizationStateCreateInfo.rasterizerDiscardEnable = GET_VK_BOOL(specification.m_EnableRasterizerDiscard);

			// Multisample state.
			m_MultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			m_MultisampleStateCreateInfo.pNext = nullptr;
			m_MultisampleStateCreateInfo.flags = 0;
			m_MultisampleStateCreateInfo.alphaToCoverageEnable = GET_VK_BOOL(specification.m_EnableAlphaCoverage);
			m_MultisampleStateCreateInfo.alphaToOneEnable = GET_VK_BOOL(specification.m_EnableAlphaToOne);
			m_MultisampleStateCreateInfo.minSampleShading = specification.m_MinSampleShading;
			m_MultisampleStateCreateInfo.pSampleMask;	// TODO
			m_MultisampleStateCreateInfo.rasterizationSamples = Utility::GetSampleCountFlagBits(m_pRasterizer->getMultisample());
			m_MultisampleStateCreateInfo.sampleShadingEnable = GET_VK_BOOL(specification.m_EnableSampleShading);

			// Depth stencil state.
			m_DepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			m_DepthStencilStateCreateInfo.pNext = nullptr;
			m_DepthStencilStateCreateInfo.flags = 0;
			m_DepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			m_DepthStencilStateCreateInfo.depthTestEnable = GET_VK_BOOL(specification.m_EnableDepthTest);
			m_DepthStencilStateCreateInfo.depthWriteEnable = GET_VK_BOOL(specification.m_EnableDepthWrite);
			m_DepthStencilStateCreateInfo.depthCompareOp = GetCompareOp(specification.m_DepthCompareLogic);

			// Dynamic state.
			m_DynamicStates = std::move(GetDynamicStates(specification.m_DynamicStateFlags));

			m_DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			m_DynamicStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_DynamicStateCreateInfo.flags = 0;
			m_DynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(m_DynamicStates.size());
			m_DynamicStateCreateInfo.pDynamicStates = m_DynamicStates.data();
		}

		void VulkanRasterizingPipeline::createPipeline()
		{
			// Resolve viewport state.
			VkRect2D rect2D = {};
			rect2D.extent.width = m_pRasterizer->getWidth();
			rect2D.extent.height = m_pRasterizer->getHeight();
			rect2D.offset = { 0, 0 };

			VkViewport viewport = {};
			viewport.width = static_cast<float>(rect2D.extent.width);
			viewport.height = static_cast<float>(rect2D.extent.height);
			viewport.maxDepth = 1.0f;
			viewport.minDepth = 0.0f;
			viewport.x = 0.0f;
			viewport.y = 0.0f;

			VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
			viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportStateCreateInfo.pNext = nullptr;
			viewportStateCreateInfo.flags = 0;
			viewportStateCreateInfo.scissorCount = 1;
			viewportStateCreateInfo.pScissors = &rect2D;
			viewportStateCreateInfo.viewportCount = 1;
			viewportStateCreateInfo.pViewports = &viewport;

			// Pipeline create info.
			VkGraphicsPipelineCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.stageCount = static_cast<uint32_t>(m_ShaderStageCreateInfo.size());
			createInfo.pStages = m_ShaderStageCreateInfo.data();
			createInfo.pVertexInputState = &m_VertexInputStateCreateInfo;
			createInfo.pInputAssemblyState = &m_InputAssemblyStateCreateInfo;
			createInfo.pTessellationState = &m_TessellationStateCreateInfo;
			createInfo.pViewportState = &viewportStateCreateInfo;
			createInfo.pRasterizationState = &m_RasterizationStateCreateInfo;
			createInfo.pMultisampleState = &m_MultisampleStateCreateInfo;
			createInfo.pDepthStencilState = &m_DepthStencilStateCreateInfo;
			createInfo.pColorBlendState = &m_ColorBlendStateCreateInfo;
			createInfo.pDynamicState = &m_DynamicStateCreateInfo;
			createInfo.layout = m_PipelineLayout;
			createInfo.renderPass = m_pRasterizer->as<VulkanRasterizer>()->getRenderPass();
			createInfo.subpass = 0;	// TODO
			createInfo.basePipelineHandle = VK_NULL_HANDLE;
			createInfo.basePipelineIndex = 0;

			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreateGraphicsPipelines(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_PipelineCache, 1, &createInfo, nullptr, &m_Pipeline), "Failed to create the pipeline!");
		}

		void VulkanRasterizingPipeline::destroyShaders()
		{
			for (const auto& shaders : m_ShaderStageCreateInfo)
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyShaderModule(getDevice().as<VulkanDevice>()->getLogicalDevice(), shaders.module, nullptr);
		}
	}
}