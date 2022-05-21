// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"
#include "Geometry.hpp"

namespace Flint
{
	/**
	 * Pipeline manager class.
	 * These managers contain a database of pipelines which can be accessed by meshes.
	 * If a pipeline does not exist for a mesh, it will either throw an exception stating that a pipeline was not found, or create one with a predefined specification.
	 */
	class PipelineManager
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param renderTarget The render target reference.
		 */
		explicit PipelineManager(RenderTarget& renderTarget) : m_RenderTarget(renderTarget) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~PipelineManager() = default;

		/**
		 * Check if a pipeline exists for a given mesh.
		 *
		 * @param mesh The mesh to check.
		 * @return The value stating if it exists or not.
		 */
		[[nodiscard]] virtual bool pipelineExists(const Mesh& mesh) const = 0;

	protected:
		RenderTarget& m_RenderTarget;
	};
}