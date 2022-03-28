// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore.hpp"
#include <glm/glm.hpp>

namespace Flint
{
	namespace Components
	{
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
			ModelViewProjectionUniform(GraphicsDevice* pDevice);

			MatrixMVP m_Matrix = {};
			UniformBuffer m_UniformBuffer;

			void UpdateUniform();
		};

		/**
		 * Texture uniform structure.
		 */
		struct Texture
		{
			std::shared_ptr<Graphics::Image> pImage = nullptr;
			std::shared_ptr<Graphics::ImageSampler> pImageSampler = nullptr;
		};
	}
}
