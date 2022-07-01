// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRasterizingPipeline.hpp"
#include "VulkanBackend/VulkanRasterizer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanRasterizingProgram.hpp"
#include "VulkanBackend/VulkanRasterizingDrawEntry.hpp"
#include "VulkanBackend/VulkanStaticModel.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

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
	VkPrimitiveTopology GetPrimitiveTopology(Flint::PrimitiveTopology topology)
	{
		switch (topology)
		{
		case Flint::PrimitiveTopology::PointList:						return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case Flint::PrimitiveTopology::LineList:						return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case Flint::PrimitiveTopology::LineStrip:						return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case Flint::PrimitiveTopology::TriangleList:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case Flint::PrimitiveTopology::TriangleStrip:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case Flint::PrimitiveTopology::TriangleFan:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		case Flint::PrimitiveTopology::LineListWithAdjacency:			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
		case Flint::PrimitiveTopology::LineStripWithAdjacency:		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
		case Flint::PrimitiveTopology::TriangleListWithAdjacency:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
		case Flint::PrimitiveTopology::TriangleStripWithAdjacency:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
		case Flint::PrimitiveTopology::PatchList:						return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		default:															throw Flint::BackendError("Invalid or Undefined primitive topology!");
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
		case Flint::CullMode::Front:									return VK_CULL_MODE_FRONT_BIT;
		case Flint::CullMode::Back:									return VK_CULL_MODE_BACK_BIT;
		case Flint::CullMode::FrontAndBack:							return VK_CULL_MODE_FRONT_AND_BACK;
		default:															throw Flint::BackendError("Invalid or Undefined cull mode!");
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
		case Flint::FrontFace::CounterClockwise:						return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case Flint::FrontFace::Clockwise:								return VK_FRONT_FACE_CLOCKWISE;
		default:															throw Flint::BackendError("Invalid or Undefined front face!");
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
		default:															throw Flint::BackendError("Invalid or Undefined polygon mode!");
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
		case Flint::ColorBlendLogic::And:								return VK_LOGIC_OP_AND;
		case Flint::ColorBlendLogic::AndReverse:						return VK_LOGIC_OP_AND_REVERSE;
		case Flint::ColorBlendLogic::Copy:							return VK_LOGIC_OP_COPY;
		case Flint::ColorBlendLogic::AndInverted:						return VK_LOGIC_OP_AND_INVERTED;
		case Flint::ColorBlendLogic::NoOperator:						return VK_LOGIC_OP_NO_OP;
		case Flint::ColorBlendLogic::XOR:								return VK_LOGIC_OP_XOR;
		case Flint::ColorBlendLogic::OR:								return VK_LOGIC_OP_OR;
		case Flint::ColorBlendLogic::NOR:								return VK_LOGIC_OP_NOR;
		case Flint::ColorBlendLogic::Equivalent:						return VK_LOGIC_OP_EQUIVALENT;
		case Flint::ColorBlendLogic::Invert:							return VK_LOGIC_OP_INVERT;
		case Flint::ColorBlendLogic::ReverseOR:						return VK_LOGIC_OP_OR_REVERSE;
		case Flint::ColorBlendLogic::CopyInverted:					return VK_LOGIC_OP_COPY_INVERTED;
		case Flint::ColorBlendLogic::InvertedOR:						return VK_LOGIC_OP_OR_INVERTED;
		case Flint::ColorBlendLogic::NAND:							return VK_LOGIC_OP_NAND;
		case Flint::ColorBlendLogic::Set:								return VK_LOGIC_OP_SET;
		default:															throw Flint::BackendError("Invalid or Undefined color blend logic!");
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
		case Flint::DepthCompareLogic::Never:							return VK_COMPARE_OP_NEVER;
		case Flint::DepthCompareLogic::Less:							return VK_COMPARE_OP_LESS;
		case Flint::DepthCompareLogic::Equal:							return VK_COMPARE_OP_EQUAL;
		case Flint::DepthCompareLogic::LessOrEqual:					return VK_COMPARE_OP_LESS_OR_EQUAL;
		case Flint::DepthCompareLogic::Greater:						return VK_COMPARE_OP_GREATER;
		case Flint::DepthCompareLogic::NotEqual:						return VK_COMPARE_OP_NOT_EQUAL;
		case Flint::DepthCompareLogic::GreaterOrEqual:				return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case Flint::DepthCompareLogic::Always:						return VK_COMPARE_OP_ALWAYS;
		default:															throw Flint::BackendError("Invalid or Undefined depth compare logic!");
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
		case Flint::ColorBlendFactor::ConstantColor:					return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case Flint::ColorBlendFactor::OneMinusConstantColor:			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		case Flint::ColorBlendFactor::ConstantAlpha:					return VK_BLEND_FACTOR_CONSTANT_ALPHA;
		case Flint::ColorBlendFactor::OneMinusConstantAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
		case Flint::ColorBlendFactor::SourceAlphaSaturate:			return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		case Flint::ColorBlendFactor::SourceOneColor:					return VK_BLEND_FACTOR_SRC1_COLOR;
		case Flint::ColorBlendFactor::OneMinusSourceOneColor:			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
		case Flint::ColorBlendFactor::SourceOneAlpha:					return VK_BLEND_FACTOR_SRC1_ALPHA;
		case Flint::ColorBlendFactor::OneMinusSourceOneAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
		default:															throw Flint::BackendError("Invalid color blend factor!");
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
		case Flint::ColorBlendOperator::Add:							return VK_BLEND_OP_ADD;
		case Flint::ColorBlendOperator::Subtract:						return VK_BLEND_OP_SUBTRACT;
		case Flint::ColorBlendOperator::ReverseSubtract:				return VK_BLEND_OP_REVERSE_SUBTRACT;
		case Flint::ColorBlendOperator::Minimum:						return VK_BLEND_OP_MIN;
		case Flint::ColorBlendOperator::Maximum:						return VK_BLEND_OP_MAX;
		case Flint::ColorBlendOperator::Zero:							return VK_BLEND_OP_ZERO_EXT;
		case Flint::ColorBlendOperator::Source:						return VK_BLEND_OP_SRC_EXT;
		case Flint::ColorBlendOperator::Destination:					return VK_BLEND_OP_DST_EXT;
		case Flint::ColorBlendOperator::SourceOver:					return VK_BLEND_OP_SRC_OVER_EXT;
		case Flint::ColorBlendOperator::DestinationOver:				return VK_BLEND_OP_DST_OVER_EXT;
		case Flint::ColorBlendOperator::SourceIn:						return VK_BLEND_OP_SRC_IN_EXT;
		case Flint::ColorBlendOperator::DestinationIn:				return VK_BLEND_OP_DST_IN_EXT;
		case Flint::ColorBlendOperator::SouceOut:						return VK_BLEND_OP_SRC_OUT_EXT;
		case Flint::ColorBlendOperator::DestinationOut:				return VK_BLEND_OP_DST_OUT_EXT;
		case Flint::ColorBlendOperator::SourceATOP:					return VK_BLEND_OP_SRC_ATOP_EXT;
		case Flint::ColorBlendOperator::DestinationATOP:				return VK_BLEND_OP_DST_ATOP_EXT;
		case Flint::ColorBlendOperator::XOR:							return VK_BLEND_OP_XOR_EXT;
		case Flint::ColorBlendOperator::Multiply:						return VK_BLEND_OP_MULTIPLY_EXT;
		case Flint::ColorBlendOperator::Screen:						return VK_BLEND_OP_SCREEN_EXT;
		case Flint::ColorBlendOperator::Overlay:						return VK_BLEND_OP_OVERLAY_EXT;
		case Flint::ColorBlendOperator::Darken:						return VK_BLEND_OP_DARKEN_EXT;
		case Flint::ColorBlendOperator::Lighten:						return VK_BLEND_OP_LIGHTEN_EXT;
		case Flint::ColorBlendOperator::ColorDodge:					return VK_BLEND_OP_COLORDODGE_EXT;
		case Flint::ColorBlendOperator::ColorBurn:					return VK_BLEND_OP_COLORBURN_EXT;
		case Flint::ColorBlendOperator::HardLight:					return VK_BLEND_OP_HARDLIGHT_EXT;
		case Flint::ColorBlendOperator::SoftLight:					return VK_BLEND_OP_SOFTLIGHT_EXT;
		case Flint::ColorBlendOperator::Difference:					return VK_BLEND_OP_DIFFERENCE_EXT;
		case Flint::ColorBlendOperator::Exclusion:					return VK_BLEND_OP_EXCLUSION_EXT;
		case Flint::ColorBlendOperator::Invert:						return VK_BLEND_OP_INVERT_EXT;
		case Flint::ColorBlendOperator::InvertRGB:					return VK_BLEND_OP_INVERT_RGB_EXT;
		case Flint::ColorBlendOperator::LinearDodge:					return VK_BLEND_OP_LINEARDODGE_EXT;
		case Flint::ColorBlendOperator::LinearBurn:					return VK_BLEND_OP_LINEARBURN_EXT;
		case Flint::ColorBlendOperator::VividLight:					return VK_BLEND_OP_VIVIDLIGHT_EXT;
		case Flint::ColorBlendOperator::LinearLight:					return VK_BLEND_OP_LINEARLIGHT_EXT;
		case Flint::ColorBlendOperator::PinLight:						return VK_BLEND_OP_PINLIGHT_EXT;
		case Flint::ColorBlendOperator::HardMix:						return VK_BLEND_OP_HARDMIX_EXT;
		case Flint::ColorBlendOperator::HSLHue:						return VK_BLEND_OP_HSL_HUE_EXT;
		case Flint::ColorBlendOperator::HSLSaturation:				return VK_BLEND_OP_HSL_SATURATION_EXT;
		case Flint::ColorBlendOperator::HSLColor:						return VK_BLEND_OP_HSL_COLOR_EXT;
		case Flint::ColorBlendOperator::HSLLuminosity:				return VK_BLEND_OP_HSL_LUMINOSITY_EXT;
		case Flint::ColorBlendOperator::Plus:							return VK_BLEND_OP_PLUS_EXT;
		case Flint::ColorBlendOperator::PlusClamped:					return VK_BLEND_OP_PLUS_CLAMPED_EXT;
		case Flint::ColorBlendOperator::PlusClampedAlpha:				return VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT;
		case Flint::ColorBlendOperator::PlusDarker:					return VK_BLEND_OP_PLUS_DARKER_EXT;
		case Flint::ColorBlendOperator::Minus:						return VK_BLEND_OP_MINUS_EXT;
		case Flint::ColorBlendOperator::MinusClamped:					return VK_BLEND_OP_MINUS_CLAMPED_EXT;
		case Flint::ColorBlendOperator::Contrast:						return VK_BLEND_OP_CONTRAST_EXT;
		case Flint::ColorBlendOperator::InvertOVG:					return VK_BLEND_OP_INVERT_OVG_EXT;
		case Flint::ColorBlendOperator::Red:							return VK_BLEND_OP_RED_EXT;
		case Flint::ColorBlendOperator::Green:						return VK_BLEND_OP_GREEN_EXT;
		case Flint::ColorBlendOperator::Blue:							return VK_BLEND_OP_BLUE_EXT;
		default:															throw Flint::BackendError("Invalid color blend operator!");
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
		VulkanRasterizingPipeline::VulkanRasterizingPipeline(const std::shared_ptr<VulkanDevice>& pDevice, const std::shared_ptr<VulkanRasterizer>& pRasterizer, const std::shared_ptr<VulkanRasterizingProgram>& pProgram, const RasterizingPipelineSpecification& specification, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler /*= nullptr*/)
			: RasterizingPipeline(pDevice, pRasterizer, pProgram, specification, std::move(pCacheHandler))
		{
			// Setup the defaults.
			setupDefaults(std::move(specification));

			// Make sure to set the object as valid.
			validate();
		}

		VulkanRasterizingPipeline::~VulkanRasterizingPipeline()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanRasterizingPipeline::terminate()
		{
			for (const auto& [hash, pipeline] : m_Pipelines)
			{
				saveCache(hash, pipeline.m_PipelineCache);
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyPipeline(getDevice().as<VulkanDevice>()->getLogicalDevice(), pipeline.m_Pipeline, nullptr);
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyPipelineCache(getDevice().as<VulkanDevice>()->getLogicalDevice(), pipeline.m_PipelineCache, nullptr);
			}

			invalidate();
		}

		void VulkanRasterizingPipeline::recreate()
		{
			// Recreate everything again.
			for (auto& [hash, pipeline] : m_Pipelines)
			{
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyPipeline(getDevice().as<VulkanDevice>()->getLogicalDevice(), pipeline.m_Pipeline, nullptr);
				pipeline.m_Pipeline = createVariation(pipeline.m_InputState, pipeline.m_PipelineCache);
			}
		}

		std::shared_ptr<Flint::DrawEntry> VulkanRasterizingPipeline::attach(const std::shared_ptr<StaticModel>& pModel)
		{
			const auto pStaticModel = pModel->as<VulkanStaticModel>();
			const auto vertexInputs = getProgram()->as<VulkanRasterizingProgram>()->getVertexInputs();
			auto pEntry = std::make_shared<VulkanRasterizingDrawEntry>(pModel);

			// Iterate over the meshes and create the required pipelines.
			for (const auto& mesh : pStaticModel->getMeshes())
			{
				const auto inputBindings = pStaticModel->getInputBindingDescriptions(mesh, vertexInputs);
				const auto inputAttributes = pStaticModel->getInputAttributeDescriptions(mesh, vertexInputs);

				// Check if the input bindings and attributes have everything we need.
				if (inputBindings.size() != vertexInputs.size() || inputAttributes.size() != vertexInputs.size())
					throw BackendError("The mesh does not contain all the vertex attributes this pipeline requires!");

				// Generate the hash which is used to uniquely identify the pipeline.
				const XXH64_hash_t hashes[] = {
					XXH64(inputBindings.data(), sizeof(VkVertexInputBindingDescription) * inputBindings.size(), 0),
					XXH64(inputAttributes.data(), sizeof(VkVertexInputAttributeDescription) * inputAttributes.size(), 0),
				};

				const auto hash = static_cast<uint64_t>(XXH64(hashes, sizeof(hashes), 0));

				// Check and create the pipeline if not available.
				if (!m_Pipelines.contains(hash))
				{
					Pipeline pipeline = {};
					pipeline.m_InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
					pipeline.m_InputState.flags = 0;
					pipeline.m_InputState.pNext = nullptr;
					pipeline.m_InputState.vertexBindingDescriptionCount = static_cast<uint32_t>(inputBindings.size());
					pipeline.m_InputState.pVertexBindingDescriptions = inputBindings.data();
					pipeline.m_InputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(inputAttributes.size());
					pipeline.m_InputState.pVertexAttributeDescriptions = inputAttributes.data();

					pipeline.m_PipelineCache = loadCache(hash);
					pipeline.m_Pipeline = createVariation(pipeline.m_InputState, pipeline.m_PipelineCache);
					saveCache(hash, pipeline.m_PipelineCache);

					m_Pipelines[hash] = pipeline;
				}

				pEntry->registerMesh(hash);
			}

			return pEntry;
		}

		VkPipelineCache VulkanRasterizingPipeline::loadCache(uint64_t identifier) const
		{
			std::vector<std::byte> buffer;

			// Load the cache if possible.
			if (m_pCacheHandler)
				buffer = m_pCacheHandler->load(identifier);

			// Create the pipeline cache.
			VkPipelineCacheCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			createInfo.pNext = VK_NULL_HANDLE;
			createInfo.flags = 0;
			createInfo.initialDataSize = buffer.size();
			createInfo.pInitialData = buffer.data();

			VkPipelineCache pipelineCache = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(getDevicePointerAs<VulkanDevice>()->getDeviceTable().vkCreatePipelineCache(getDevicePointerAs<VulkanDevice>()->getLogicalDevice(), &createInfo, nullptr, &pipelineCache), "Failed to create the pipeline cache!");

			return pipelineCache;
		}

		void VulkanRasterizingPipeline::saveCache(uint64_t identifier, VkPipelineCache cache) const
		{
			// Return if we don't have anything to save.
			if (cache == VK_NULL_HANDLE)
				return;

			// Load cache data.
			size_t cacheSize = 0;
			FLINT_VK_ASSERT(getDevicePointerAs<VulkanDevice>()->getDeviceTable().vkGetPipelineCacheData(getDevicePointerAs<VulkanDevice>()->getLogicalDevice(), cache, &cacheSize, nullptr), "Failed to get the pipeline cache size!");

			auto buffer = std::vector<std::byte>(cacheSize);
			FLINT_VK_ASSERT(getDevicePointerAs<VulkanDevice>()->getDeviceTable().vkGetPipelineCacheData(getDevicePointerAs<VulkanDevice>()->getLogicalDevice(), cache, &cacheSize, buffer.data()), "Failed to get the pipeline cache data!");

			// Store the cache if possible.
			if (m_pCacheHandler)
				m_pCacheHandler->store(identifier, buffer);
		}

		void VulkanRasterizingPipeline::setupDefaults(const RasterizingPipelineSpecification& specification)
		{
			// Setup the shader stages.
			m_ShaderStageCreateInfo = getProgram()->as<VulkanRasterizingProgram>()->getPipelineShaderStageCreateInfos();

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

		VkPipeline VulkanRasterizingPipeline::createVariation(const VkPipelineVertexInputStateCreateInfo& inputState, VkPipelineCache cache)
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
			createInfo.pVertexInputState = &inputState;
			createInfo.pInputAssemblyState = &m_InputAssemblyStateCreateInfo;
			createInfo.pTessellationState = &m_TessellationStateCreateInfo;
			createInfo.pViewportState = &viewportStateCreateInfo;
			createInfo.pRasterizationState = &m_RasterizationStateCreateInfo;
			createInfo.pMultisampleState = &m_MultisampleStateCreateInfo;
			createInfo.pDepthStencilState = &m_DepthStencilStateCreateInfo;
			createInfo.pColorBlendState = &m_ColorBlendStateCreateInfo;
			createInfo.pDynamicState = &m_DynamicStateCreateInfo;
			createInfo.layout = m_pProgram->as<VulkanRasterizingProgram>()->getPipelineLayout();
			createInfo.renderPass = m_pRasterizer->as<VulkanRasterizer>()->getRenderPass();
			createInfo.subpass = 0;	// TODO
			createInfo.basePipelineHandle = VK_NULL_HANDLE;
			createInfo.basePipelineIndex = 0;

			VkPipeline pipeline = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreateGraphicsPipelines(getDevice().as<VulkanDevice>()->getLogicalDevice(), cache, 1, &createInfo, nullptr, &pipeline), "Failed to create the pipeline!");

			return pipeline;
		}
	}
}