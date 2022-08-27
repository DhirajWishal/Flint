// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/VulkanBackend/VulkanRasterizingDrawEntry.hpp"
#include "Flint/VulkanBackend/VulkanRasterizingPipeline.hpp"

namespace Flint
{
	namespace Backend
	{
		Flint::Backend::DrawInstance VulkanRasterizingDrawEntry::instance(const glm::vec3& position /*= glm::vec3(0.0f)*/, const glm::vec3& rotation /*= glm::vec3(0.0f)*/, const glm::vec3& scale /*= glm::vec3(1.0f)*/)
		{
			auto& instance = m_DrawInstances.emplace_back();
			instance.m_InstanceID = m_Index++;
			instance.m_Position = position;
			instance.m_Rotation = rotation;
			instance.m_Scale = scale;

			return instance;
		}

		void VulkanRasterizingDrawEntry::registerMesh(uint64_t pipelineHash, uint64_t resourceHash)
		{
			m_pPipeline->notifyRenderTarget();
			m_MeshDrawers.emplace_back(pipelineHash, resourceHash);
		}
	}
}