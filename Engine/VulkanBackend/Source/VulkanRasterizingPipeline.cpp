// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRasterizingPipeline.hpp"
#include "VulkanBackend/VulkanRasterizer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanRasterizingProgram.hpp"
#include "VulkanBackend/VulkanRasterizingDrawEntry.hpp"
#include "VulkanBackend/VulkanStaticModel.hpp"
#include "VulkanBackend/VulkanCommandBuffers.hpp"

#include <Optick.h>

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
	VkPrimitiveTopology GetPrimitiveTopology(Flint::Backend::PrimitiveTopology topology)
	{
		switch (topology)
		{
		case Flint::Backend::PrimitiveTopology::PointList:						return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case Flint::Backend::PrimitiveTopology::LineList:						return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case Flint::Backend::PrimitiveTopology::LineStrip:						return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case Flint::Backend::PrimitiveTopology::TriangleList:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case Flint::Backend::PrimitiveTopology::TriangleStrip:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case Flint::Backend::PrimitiveTopology::TriangleFan:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		case Flint::Backend::PrimitiveTopology::LineListWithAdjacency:			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
		case Flint::Backend::PrimitiveTopology::LineStripWithAdjacency:			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
		case Flint::Backend::PrimitiveTopology::TriangleListWithAdjacency:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
		case Flint::Backend::PrimitiveTopology::TriangleStripWithAdjacency:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
		case Flint::Backend::PrimitiveTopology::PatchList:						return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		default:																throw Flint::BackendError("Invalid or Undefined primitive topology!");
		}
	}

	/**
	 * Get the cull mode.
	 *
	 * @param cull The flint cull mode.
	 * @return The Vulkan cull mode.
	 */
	VkCullModeFlags GetCullMode(Flint::Backend::CullMode cull)
	{
		switch (cull)
		{
		case Flint::Backend::CullMode::None:									return VK_CULL_MODE_NONE;
		case Flint::Backend::CullMode::Front:									return VK_CULL_MODE_FRONT_BIT;
		case Flint::Backend::CullMode::Back:									return VK_CULL_MODE_BACK_BIT;
		case Flint::Backend::CullMode::FrontAndBack:							return VK_CULL_MODE_FRONT_AND_BACK;
		default:																throw Flint::BackendError("Invalid or Undefined cull mode!");
		}
	}

	/**
	 * Get the front face.
	 *
	 * @param face The flint front face.
	 * @return The Vulkan front face.
	 */
	VkFrontFace GetFrontFace(Flint::Backend::FrontFace face)
	{
		switch (face)
		{
		case Flint::Backend::FrontFace::CounterClockwise:						return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case Flint::Backend::FrontFace::Clockwise:								return VK_FRONT_FACE_CLOCKWISE;
		default:																throw Flint::BackendError("Invalid or Undefined front face!");
		}
	}

	/**
	 * Get the polygon mode.
	 *
	 * @param mode The flint polygon mode.
	 * @return The Vulkan polygon mode.
	 */
	VkPolygonMode GetPolygonMode(Flint::Backend::PolygonMode mode)
	{
		switch (mode)
		{
		case Flint::Backend::PolygonMode::Fill:									return VK_POLYGON_MODE_FILL;
		case Flint::Backend::PolygonMode::Line:									return VK_POLYGON_MODE_LINE;
		case Flint::Backend::PolygonMode::Point:								return VK_POLYGON_MODE_POINT;
		default:																throw Flint::BackendError("Invalid or Undefined polygon mode!");
		}
	}

	/**
	 * Get the logic operator.
	 *
	 * @param logic The flint logic.
	 * @return The Vulkan logic.
	 */
	VkLogicOp GetLogicOp(Flint::Backend::ColorBlendLogic logic)
	{
		switch (logic)
		{
		case Flint::Backend::ColorBlendLogic::Clear:							return VK_LOGIC_OP_CLEAR;
		case Flint::Backend::ColorBlendLogic::And:								return VK_LOGIC_OP_AND;
		case Flint::Backend::ColorBlendLogic::AndReverse:						return VK_LOGIC_OP_AND_REVERSE;
		case Flint::Backend::ColorBlendLogic::Copy:								return VK_LOGIC_OP_COPY;
		case Flint::Backend::ColorBlendLogic::AndInverted:						return VK_LOGIC_OP_AND_INVERTED;
		case Flint::Backend::ColorBlendLogic::NoOperator:						return VK_LOGIC_OP_NO_OP;
		case Flint::Backend::ColorBlendLogic::XOR:								return VK_LOGIC_OP_XOR;
		case Flint::Backend::ColorBlendLogic::OR:								return VK_LOGIC_OP_OR;
		case Flint::Backend::ColorBlendLogic::NOR:								return VK_LOGIC_OP_NOR;
		case Flint::Backend::ColorBlendLogic::Equivalent:						return VK_LOGIC_OP_EQUIVALENT;
		case Flint::Backend::ColorBlendLogic::Invert:							return VK_LOGIC_OP_INVERT;
		case Flint::Backend::ColorBlendLogic::ReverseOR:						return VK_LOGIC_OP_OR_REVERSE;
		case Flint::Backend::ColorBlendLogic::CopyInverted:						return VK_LOGIC_OP_COPY_INVERTED;
		case Flint::Backend::ColorBlendLogic::InvertedOR:						return VK_LOGIC_OP_OR_INVERTED;
		case Flint::Backend::ColorBlendLogic::NAND:								return VK_LOGIC_OP_NAND;
		case Flint::Backend::ColorBlendLogic::Set:								return VK_LOGIC_OP_SET;
		default:																throw Flint::BackendError("Invalid or Undefined color blend logic!");
		}
	}

	/**
	 * Get the compare operator.
	 *
	 * @param logic the flint logic.
	 * @return The Vulkan logic operator.
	 */
	VkCompareOp GetCompareOp(Flint::Backend::DepthCompareLogic logic)
	{
		switch (logic)
		{
		case Flint::Backend::DepthCompareLogic::Never:							return VK_COMPARE_OP_NEVER;
		case Flint::Backend::DepthCompareLogic::Less:							return VK_COMPARE_OP_LESS;
		case Flint::Backend::DepthCompareLogic::Equal:							return VK_COMPARE_OP_EQUAL;
		case Flint::Backend::DepthCompareLogic::LessOrEqual:					return VK_COMPARE_OP_LESS_OR_EQUAL;
		case Flint::Backend::DepthCompareLogic::Greater:						return VK_COMPARE_OP_GREATER;
		case Flint::Backend::DepthCompareLogic::NotEqual:						return VK_COMPARE_OP_NOT_EQUAL;
		case Flint::Backend::DepthCompareLogic::GreaterOrEqual:					return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case Flint::Backend::DepthCompareLogic::Always:							return VK_COMPARE_OP_ALWAYS;
		default:																throw Flint::BackendError("Invalid or Undefined depth compare logic!");
		}
	}

	/**
	 * Get the dynamic states.
	 *
	 * @param flags The flint flags.
	 * @return The Vulkan flags.
	 */
	std::vector<VkDynamicState> GetDynamicStates(Flint::Backend::DynamicStateFlags flags)
	{
		std::vector<VkDynamicState> states;
		if (flags & Flint::Backend::DynamicStateFlags::ViewPort) states.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		if (flags & Flint::Backend::DynamicStateFlags::Scissor) states.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
		if (flags & Flint::Backend::DynamicStateFlags::LineWidth) states.emplace_back(VK_DYNAMIC_STATE_LINE_WIDTH);
		if (flags & Flint::Backend::DynamicStateFlags::DepthBias) states.emplace_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
		if (flags & Flint::Backend::DynamicStateFlags::BlendConstants) states.emplace_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
		if (flags & Flint::Backend::DynamicStateFlags::DepthBounds) states.emplace_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);

		return states;
	}

	/**
	 * Get the blend factor.
	 *
	 * @param factor The flint factor.
	 * @return The Vulkan factor.
	 */
	VkBlendFactor GetBlendFactor(Flint::Backend::ColorBlendFactor factor)
	{
		switch (factor)
		{
		case Flint::Backend::ColorBlendFactor::Zero:							return VK_BLEND_FACTOR_ZERO;
		case Flint::Backend::ColorBlendFactor::One:								return VK_BLEND_FACTOR_ONE;
		case Flint::Backend::ColorBlendFactor::SourceColor:						return VK_BLEND_FACTOR_SRC_COLOR;
		case Flint::Backend::ColorBlendFactor::OneMinusSourceColor:				return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case Flint::Backend::ColorBlendFactor::DestinationColor:				return VK_BLEND_FACTOR_DST_COLOR;
		case Flint::Backend::ColorBlendFactor::OneMinusDestinationColor:		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case Flint::Backend::ColorBlendFactor::SourceAlpha:						return VK_BLEND_FACTOR_SRC_ALPHA;
		case Flint::Backend::ColorBlendFactor::OneMinusSourceAlpha:				return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case Flint::Backend::ColorBlendFactor::DestinationAlpha:				return VK_BLEND_FACTOR_DST_ALPHA;
		case Flint::Backend::ColorBlendFactor::OneMinusDestinationAlpha:		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case Flint::Backend::ColorBlendFactor::ConstantColor:					return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case Flint::Backend::ColorBlendFactor::OneMinusConstantColor:			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		case Flint::Backend::ColorBlendFactor::ConstantAlpha:					return VK_BLEND_FACTOR_CONSTANT_ALPHA;
		case Flint::Backend::ColorBlendFactor::OneMinusConstantAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
		case Flint::Backend::ColorBlendFactor::SourceAlphaSaturate:				return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		case Flint::Backend::ColorBlendFactor::SourceOneColor:					return VK_BLEND_FACTOR_SRC1_COLOR;
		case Flint::Backend::ColorBlendFactor::OneMinusSourceOneColor:			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
		case Flint::Backend::ColorBlendFactor::SourceOneAlpha:					return VK_BLEND_FACTOR_SRC1_ALPHA;
		case Flint::Backend::ColorBlendFactor::OneMinusSourceOneAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
		default:																throw Flint::BackendError("Invalid color blend factor!");
		}
	}

	/**
	 * Get the blend operator.
	 *
	 * @param oprtr The flint operator.
	 * @return The Vulkan blend operator.
	 */
	VkBlendOp GetBlendOp(Flint::Backend::ColorBlendOperator oprtr)
	{
		switch (oprtr)
		{
		case Flint::Backend::ColorBlendOperator::Add:							return VK_BLEND_OP_ADD;
		case Flint::Backend::ColorBlendOperator::Subtract:						return VK_BLEND_OP_SUBTRACT;
		case Flint::Backend::ColorBlendOperator::ReverseSubtract:				return VK_BLEND_OP_REVERSE_SUBTRACT;
		case Flint::Backend::ColorBlendOperator::Minimum:						return VK_BLEND_OP_MIN;
		case Flint::Backend::ColorBlendOperator::Maximum:						return VK_BLEND_OP_MAX;
		case Flint::Backend::ColorBlendOperator::Zero:							return VK_BLEND_OP_ZERO_EXT;
		case Flint::Backend::ColorBlendOperator::Source:						return VK_BLEND_OP_SRC_EXT;
		case Flint::Backend::ColorBlendOperator::Destination:					return VK_BLEND_OP_DST_EXT;
		case Flint::Backend::ColorBlendOperator::SourceOver:					return VK_BLEND_OP_SRC_OVER_EXT;
		case Flint::Backend::ColorBlendOperator::DestinationOver:				return VK_BLEND_OP_DST_OVER_EXT;
		case Flint::Backend::ColorBlendOperator::SourceIn:						return VK_BLEND_OP_SRC_IN_EXT;
		case Flint::Backend::ColorBlendOperator::DestinationIn:					return VK_BLEND_OP_DST_IN_EXT;
		case Flint::Backend::ColorBlendOperator::SouceOut:						return VK_BLEND_OP_SRC_OUT_EXT;
		case Flint::Backend::ColorBlendOperator::DestinationOut:				return VK_BLEND_OP_DST_OUT_EXT;
		case Flint::Backend::ColorBlendOperator::SourceATOP:					return VK_BLEND_OP_SRC_ATOP_EXT;
		case Flint::Backend::ColorBlendOperator::DestinationATOP:				return VK_BLEND_OP_DST_ATOP_EXT;
		case Flint::Backend::ColorBlendOperator::XOR:							return VK_BLEND_OP_XOR_EXT;
		case Flint::Backend::ColorBlendOperator::Multiply:						return VK_BLEND_OP_MULTIPLY_EXT;
		case Flint::Backend::ColorBlendOperator::Screen:						return VK_BLEND_OP_SCREEN_EXT;
		case Flint::Backend::ColorBlendOperator::Overlay:						return VK_BLEND_OP_OVERLAY_EXT;
		case Flint::Backend::ColorBlendOperator::Darken:						return VK_BLEND_OP_DARKEN_EXT;
		case Flint::Backend::ColorBlendOperator::Lighten:						return VK_BLEND_OP_LIGHTEN_EXT;
		case Flint::Backend::ColorBlendOperator::ColorDodge:					return VK_BLEND_OP_COLORDODGE_EXT;
		case Flint::Backend::ColorBlendOperator::ColorBurn:						return VK_BLEND_OP_COLORBURN_EXT;
		case Flint::Backend::ColorBlendOperator::HardLight:						return VK_BLEND_OP_HARDLIGHT_EXT;
		case Flint::Backend::ColorBlendOperator::SoftLight:						return VK_BLEND_OP_SOFTLIGHT_EXT;
		case Flint::Backend::ColorBlendOperator::Difference:					return VK_BLEND_OP_DIFFERENCE_EXT;
		case Flint::Backend::ColorBlendOperator::Exclusion:						return VK_BLEND_OP_EXCLUSION_EXT;
		case Flint::Backend::ColorBlendOperator::Invert:						return VK_BLEND_OP_INVERT_EXT;
		case Flint::Backend::ColorBlendOperator::InvertRGB:						return VK_BLEND_OP_INVERT_RGB_EXT;
		case Flint::Backend::ColorBlendOperator::LinearDodge:					return VK_BLEND_OP_LINEARDODGE_EXT;
		case Flint::Backend::ColorBlendOperator::LinearBurn:					return VK_BLEND_OP_LINEARBURN_EXT;
		case Flint::Backend::ColorBlendOperator::VividLight:					return VK_BLEND_OP_VIVIDLIGHT_EXT;
		case Flint::Backend::ColorBlendOperator::LinearLight:					return VK_BLEND_OP_LINEARLIGHT_EXT;
		case Flint::Backend::ColorBlendOperator::PinLight:						return VK_BLEND_OP_PINLIGHT_EXT;
		case Flint::Backend::ColorBlendOperator::HardMix:						return VK_BLEND_OP_HARDMIX_EXT;
		case Flint::Backend::ColorBlendOperator::HSLHue:						return VK_BLEND_OP_HSL_HUE_EXT;
		case Flint::Backend::ColorBlendOperator::HSLSaturation:					return VK_BLEND_OP_HSL_SATURATION_EXT;
		case Flint::Backend::ColorBlendOperator::HSLColor:						return VK_BLEND_OP_HSL_COLOR_EXT;
		case Flint::Backend::ColorBlendOperator::HSLLuminosity:					return VK_BLEND_OP_HSL_LUMINOSITY_EXT;
		case Flint::Backend::ColorBlendOperator::Plus:							return VK_BLEND_OP_PLUS_EXT;
		case Flint::Backend::ColorBlendOperator::PlusClamped:					return VK_BLEND_OP_PLUS_CLAMPED_EXT;
		case Flint::Backend::ColorBlendOperator::PlusClampedAlpha:				return VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT;
		case Flint::Backend::ColorBlendOperator::PlusDarker:					return VK_BLEND_OP_PLUS_DARKER_EXT;
		case Flint::Backend::ColorBlendOperator::Minus:							return VK_BLEND_OP_MINUS_EXT;
		case Flint::Backend::ColorBlendOperator::MinusClamped:					return VK_BLEND_OP_MINUS_CLAMPED_EXT;
		case Flint::Backend::ColorBlendOperator::Contrast:						return VK_BLEND_OP_CONTRAST_EXT;
		case Flint::Backend::ColorBlendOperator::InvertOVG:						return VK_BLEND_OP_INVERT_OVG_EXT;
		case Flint::Backend::ColorBlendOperator::Red:							return VK_BLEND_OP_RED_EXT;
		case Flint::Backend::ColorBlendOperator::Green:							return VK_BLEND_OP_GREEN_EXT;
		case Flint::Backend::ColorBlendOperator::Blue:							return VK_BLEND_OP_BLUE_EXT;
		default:																throw Flint::BackendError("Invalid color blend operator!");
		}
	}

	/**
	 * Get the color component flags.
	 *
	 * @param mask The color write mask.
	 * @return The VUlkan mask.
	 */
	VkColorComponentFlags GetComponentFlags(Flint::Backend::ColorWriteMask mask)
	{
		VkColorComponentFlags flags = 0;
		if (mask & Flint::Backend::ColorWriteMask::R) flags |= VK_COLOR_COMPONENT_R_BIT;
		if (mask & Flint::Backend::ColorWriteMask::G) flags |= VK_COLOR_COMPONENT_G_BIT;
		if (mask & Flint::Backend::ColorWriteMask::B) flags |= VK_COLOR_COMPONENT_B_BIT;
		if (mask & Flint::Backend::ColorWriteMask::A) flags |= VK_COLOR_COMPONENT_A_BIT;

		return flags;
	}
}

namespace Flint
{
	namespace Backend
	{
		VulkanRasterizingPipeline::VulkanRasterizingPipeline(const std::shared_ptr<VulkanDevice>& pDevice, const std::shared_ptr<VulkanRasterizer>& pRasterizer, const std::shared_ptr<VulkanRasterizingProgram>& pProgram, const RasterizingPipelineSpecification& specification, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler /*= nullptr*/)
			: RasterizingPipeline(pDevice, pRasterizer, pProgram, specification, std::move(pCacheHandler))
			, m_DescriptorSetManager(pDevice, pRasterizer->getFrameCount())
			, m_WorkerThread(&VulkanRasterizingPipeline::worker, this)
			, m_pSecondaryCommandBuffers(pRasterizer->getCommandBuffers()->createChild())
		{
			OPTICK_EVENT();

			// Setup the defaults.
			setupDefaults(specification);

			// Setup the descriptor set manager.
			m_DescriptorSetManager.setup(pProgram->getLayoutBindings(), pProgram->getPoolSizes(), pProgram->getDescriptorSetLayout());

			// Make sure to set the object as valid.
			validate();
		}

		VulkanRasterizingPipeline::~VulkanRasterizingPipeline()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanRasterizingPipeline::terminate()
		{
			OPTICK_EVENT();

			for (const auto& [hash, pipeline] : m_Pipelines)
			{
				saveCache(hash, pipeline.m_PipelineCache);
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyPipeline(getDevice().as<VulkanDevice>()->getLogicalDevice(), pipeline.m_Pipeline, nullptr);
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyPipelineCache(getDevice().as<VulkanDevice>()->getLogicalDevice(), pipeline.m_PipelineCache, nullptr);
			}

			m_pSecondaryCommandBuffers->terminate();
			m_DescriptorSetManager.destroy();
			terminateWorker();
			invalidate();
		}

		void VulkanRasterizingPipeline::recreate()
		{
			OPTICK_EVENT();

			// Recreate everything again.
			for (auto& [hash, pipeline] : m_Pipelines)
			{
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyPipeline(getDevice().as<VulkanDevice>()->getLogicalDevice(), pipeline.m_Pipeline, nullptr);

				VkPipelineVertexInputStateCreateInfo inputState = {};
				inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				inputState.flags = 0;
				inputState.pNext = nullptr;
				inputState.vertexBindingDescriptionCount = static_cast<uint32_t>(pipeline.m_InputBindings.size());
				inputState.pVertexBindingDescriptions = pipeline.m_InputBindings.data();
				inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(pipeline.m_InputAttributes.size());
				inputState.pVertexAttributeDescriptions = pipeline.m_InputAttributes.data();
				pipeline.m_Pipeline = createVariation(std::move(inputState), pipeline.m_PipelineCache);
			}
		}

		std::shared_ptr<Flint::Backend::DrawEntry> VulkanRasterizingPipeline::attach(const std::shared_ptr<StaticModel>& pModel, ResourceBinder&& binder)
		{
			OPTICK_EVENT();

			const auto pStaticModel = pModel->as<VulkanStaticModel>();
			const auto vertexInputs = getProgram()->as<VulkanRasterizingProgram>()->getVertexInputs();
			const auto& bindingMap = getProgram()->getBindingMap();

			auto pEntry = std::make_shared<VulkanRasterizingDrawEntry>(pModel, shared_from_this());

			// Iterate over the meshes and create the required pipelines.
			for (const auto& mesh : pStaticModel->getMeshes())
			{
				// Prepare the required resources.
				const auto bindingTable = binder(*pModel, mesh, bindingMap);
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

				const auto pipelineHash = static_cast<uint64_t>(XXH64(hashes, sizeof(hashes), 0));

				// Check and create the pipeline if not available.
				if (!m_Pipelines.contains(pipelineHash))
				{
					Pipeline pipeline = {};
					pipeline.m_InputBindings = inputBindings;
					pipeline.m_InputAttributes = inputAttributes;

					VkPipelineVertexInputStateCreateInfo inputState = {};
					inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
					inputState.flags = 0;
					inputState.pNext = nullptr;
					inputState.vertexBindingDescriptionCount = static_cast<uint32_t>(pipeline.m_InputBindings.size());
					inputState.pVertexBindingDescriptions = pipeline.m_InputBindings.data();
					inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(pipeline.m_InputAttributes.size());
					inputState.pVertexAttributeDescriptions = pipeline.m_InputAttributes.data();

					pipeline.m_PipelineCache = loadCache(pipelineHash);
					pipeline.m_Pipeline = createVariation(std::move(inputState), pipeline.m_PipelineCache);
					saveCache(pipelineHash, pipeline.m_PipelineCache);

					m_Pipelines[pipelineHash] = pipeline;
				}

				// Setup resources.
				m_DescriptorSetManager.registerTable(bindingTable);
				pEntry->registerMesh(pipelineHash, bindingTable.generateHash());
			}

			// Register the draw call callback.
			m_DrawCalls.emplace_back([this, pEntry, vertexInputs, pStaticModel](const VulkanCommandBuffers& commandBuffers, uint32_t frameIndex)
				{
					commandBuffers.bindVertexBuffers(pStaticModel->getVertexStorage(), vertexInputs);
					commandBuffers.bindIndexBuffer(pStaticModel->getIndexBufferHandle());

					const auto& meshDrawers = pEntry->getMeshDrawers();
					for (uint32_t i = 0; i < meshDrawers.size(); i++)
					{
						const auto meshDrawer = meshDrawers[i];
						const auto& mesh = pStaticModel->getMeshes()[i];

						commandBuffers.bindRasterizingPipeline(getPipelineHandle(meshDrawer.m_PipelineHash));
						commandBuffers.bindDescriptor(this, getDescriptorSetManager().getDescriptorSet(meshDrawers[i].m_ResourceHash, frameIndex));
						commandBuffers.drawIndexed(mesh.m_IndexCount, mesh.m_IndexOffset, pEntry->getInstanceCount(), mesh.m_VertexOffset);
					}
				}
			);

			return m_pDrawEntries.emplace_back(std::move(pEntry));
		}

		VkPipelineCache VulkanRasterizingPipeline::loadCache(uint64_t identifier) const
		{
			OPTICK_EVENT();

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
			OPTICK_EVENT();

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

		void VulkanRasterizingPipeline::notifyRenderTarget()
		{
			getRasterizer()->toggleNeedToUpdate();
		}

		void VulkanRasterizingPipeline::draw(VkCommandBufferInheritanceInfo inheritanceInfo, uint32_t frameIndex, bool shouldWait /*= false*/)
		{
			{
				[[maybe_unused]] const auto lock = std::scoped_lock(m_WorkerMutex);
				m_WorkerPayload.m_InheritanceInfo = inheritanceInfo;
				m_WorkerPayload.m_FrameIndex = frameIndex;
			}

			m_Updated = false;
			m_ConditionVariable.notify_one();

			if (shouldWait)
			{
				auto uniqueLock = std::unique_lock(m_WorkerMutex);
				m_ConditionVariable.wait(uniqueLock);
			}
		}

		void VulkanRasterizingPipeline::issueDrawCalls(const VulkanCommandBuffers& commandBuffers, uint32_t frameIndex) const
		{
			OPTICK_EVENT();

			for (const auto& drawCall : m_DrawCalls)
				drawCall(commandBuffers, frameIndex);
		}

		void VulkanRasterizingPipeline::setupDefaults(const RasterizingPipelineSpecification& specification)
		{
			OPTICK_EVENT();

			// Setup the shader stages.
			m_ShaderStageCreateInfo = getProgram()->as<VulkanRasterizingProgram>()->getPipelineShaderStageCreateInfos();

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

		VkPipeline VulkanRasterizingPipeline::createVariation(VkPipelineVertexInputStateCreateInfo&& inputState, VkPipelineCache cache)
		{
			OPTICK_EVENT();

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

		void VulkanRasterizingPipeline::worker()
		{
			OPTICK_THREAD("Rasterizing Pipeline Worker");

			do
			{
				auto uniqueLock = std::unique_lock(m_WorkerMutex);
				m_ConditionVariable.wait(uniqueLock);

				// Make sure to only run if we need to.
				if (m_ShouldRun)
				{
					m_pSecondaryCommandBuffers->begin(&m_WorkerPayload.m_InheritanceInfo);
					issueDrawCalls(*m_pSecondaryCommandBuffers, m_WorkerPayload.m_FrameIndex);
					m_pSecondaryCommandBuffers->end();
					m_pSecondaryCommandBuffers->execute();
					m_pSecondaryCommandBuffers->next();

					// Notify that we finished execution.
					m_ConditionVariable.notify_one();
				}

			} while (m_ShouldRun);
		}

		void VulkanRasterizingPipeline::terminateWorker()
		{
			m_ShouldRun = false;
			m_ConditionVariable.notify_one();
			m_WorkerThread.join();
		}
	}
}