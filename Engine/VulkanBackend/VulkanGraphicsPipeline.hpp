// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Rasterizer.hpp"
#include "VulkanPipeline.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan graphics pipeline class.
		 * This pipeline is used to perform raster graphics.
		 */
		class VulkanGraphicsPipeline final : public VulkanPipeline
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param engine The engine to which the pipeline is bound to.
			 * @param name The name of the pipeline.
			 * @param specification The pipeline specification.
			 */
			explicit VulkanGraphicsPipeline(VulkanEngine& engine, std::string&& name, RasterizingPipelineSpecification&& specification);

			/**
			 * Destructor.
			 */
			~VulkanGraphicsPipeline() override;

		private:
			/**
			 * Create the pipeline layout.
			 */
			void createPipelineLayout();

		private:
			RasterizingPipelineSpecification m_Specification = {};
		};
	}
}