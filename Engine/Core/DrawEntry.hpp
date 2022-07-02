// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Entity.hpp"
#include <glm/glm.hpp>

namespace Flint
{
	/**
	 * Draw instance structure.
	 * This holds all the necessary information regarding a single draw instance.
	 */
	struct DrawInstance final
	{
		uint32_t m_InstanceID = 0;
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f);
		glm::vec3 m_Scale = glm::vec3(1.0f);
	};

	/**
	 * Draw entry class.
	 * This class contains information about a single model which is bound to a pipeline (rasterizing or ray tracing) and is used to instance it.
	 */
	class DrawEntry
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pEntity The entity pointer.
		 */
		explicit DrawEntry(const std::shared_ptr<Entity>& pEntity);

		/**
		 * Create a new draw instance.
		 *
		 * @param position The position to which the object is instanced at. Default is (0, 0, 0).
		 * @param rotation The rotation of the instanced model. Default is (0, 0, 0).
		 * @param scale The scale of the instanced model. Default is (1, 1, 1).
		 * @return The draw instance.
		 */
		[[nodiscard]] virtual DrawInstance instance(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f)) = 0;

		/**
		 * Get the entity pointer.
		 *
		 * @return The entity pointer.
		 */
		[[nodiscard]] const Entity* getEntity() const { return m_pEntity.get(); }

		/**
		 * Get the created draw instances.
		 *
		 * @return The instances.
		 */
		[[nodiscard]] const std::vector<DrawInstance>& getDrawInstances() const { return m_DrawInstances; }

		/**
		 * Get the number of instances to draw.
		 *
		 * @return The instance count.
		 */
		[[nodiscard]] uint64_t getInstanceCount() const { return m_DrawInstances.size(); }

	protected:
		std::shared_ptr<Entity> m_pEntity = nullptr;
		std::vector<DrawInstance> m_DrawInstances;
	};
}