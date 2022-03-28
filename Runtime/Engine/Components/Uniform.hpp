// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ComponentBase.hpp"
#include "GraphicsCore/Buffer.hpp"
#include <glm/glm.hpp>

namespace Flint
{
	namespace Components
	{
		/**
		 * Uniform buffer structure.
		 */
		struct UniformBuffer
		{
			std::unique_ptr<Buffer> pUniformBuffer = nullptr;
		};

		/**
		 * The MVP matrix.
		 */
		struct MatrixMVP
		{
			glm::mat4 m_Model = glm::mat4(1.0f);
			glm::mat4 m_View = glm::mat4(1.0f);
			glm::mat4 m_Projection = glm::mat4(1.0f);
		};

		/**
		 * Model view projection uniform.
		 */
		struct ModelViewProjectionUniform
		{
			ModelViewProjectionUniform() = default;
			ModelViewProjectionUniform(Device* pDevice);

			MatrixMVP m_Matrix = {};
			std::unique_ptr<Buffer> pUniformBuffer = nullptr;

			void UpdateUniform();
		};
	}
}
