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
			std::shared_ptr<Buffer> pUniformBuffer = nullptr;
		};

		/**
		 * The MVP matrix.
		 */
		struct MatrixMVP
		{
			glm::mat4 mModel = glm::mat4(1.0f);
			glm::mat4 mView = glm::mat4(1.0f);
			glm::mat4 mProjection = glm::mat4(1.0f);
		};

		/**
		 * Model view projection uniform.
		 */
		struct ModelViewProjectionUniform
		{
			ModelViewProjectionUniform() = default;
			ModelViewProjectionUniform(const std::shared_ptr<Device>& pDevice);

			MatrixMVP mMatrix = {};
			std::shared_ptr<Buffer> pUniformBuffer = nullptr;

			void UpdateUniform();
		};
	}
}
