// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DrawEntry.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan rasterizing draw entry class.
		 */
		class VulkanRasterizingDrawEntry final : public DrawEntry
		{
			/**
			 * Mesh drawer structure.
			 * This contains all the necessary data required to render a single mesh.
			 */
			struct MeshDrawer final
			{
				uint64_t m_PipelineHash = 0;
				uint64_t m_ResourceHash = 0;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pEntity The entity pointer.
			 */
			explicit VulkanRasterizingDrawEntry(const std::shared_ptr<Entity>& pEntity) : DrawEntry(pEntity) {}

			/**
			 * Create a new draw instance.
			 *
			 * @param position The position to which the object is instanced at. Default is (0, 0, 0).
			 * @param rotation The rotation of the instanced model. Default is (0, 0, 0).
			 * @param scale The scale of the instanced model. Default is (1, 1, 1).
			 * @return The draw instance.
			 */
			[[nodiscard]] DrawInstance instance(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f)) override;

			/**
			 * Register a mesh to the entry.
			 *
			 * @param pipelineHash The pipeline hash used to render.
			 * @param resourceHash The resource hash used to properly bind the requested resources.
			 */
			void registerMesh(uint64_t pipelineHash, uint64_t resourceHash);

		private:
			std::vector<MeshDrawer> m_MeshDrawers;
			uint32_t m_Index = 0;
		};
	}
}