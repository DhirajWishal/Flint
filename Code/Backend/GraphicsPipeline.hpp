// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Shader.hpp"
#include "RenderTarget.hpp"

namespace Flint
{
	/**
	 * Flint graphics pipeline.
	 * This object uses rasterization to draw graphics.
	 *
	 * Graphics pipelines have 5 main stages, with 3 optional stages.
	 * 1. Vertex.
	 * 2. Tessellation control (optional).
	 * 3. Tessellation evaluation (optional).
	 * 4. Geometry (optional).
	 * 5. Fragment.
	 */
	class GraphicsPipeline : public Pipeline
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 * @param pRenderTarget: The render target pointer.
		 * @param pVertexShader: The vertex shader pointer.
		 * @param pTessellationControlShader: The tessellation control shader (optional).
		 * @param pTessellationEvaluationShader: The tessellation evaluation shader (optional).
		 * @param pGeometryShader: The geometry shader (optional).
		 * @param pFragmentShader: The fragment shader.
		 */
		GraphicsPipeline(
			const std::shared_ptr<Device>& pDevice,
			const std::shared_ptr<RenderTarget>& pRenderTarget,
			const std::shared_ptr<Shader>& pVertexShader,
			const std::shared_ptr<Shader>& pTessellationControlShader,
			const std::shared_ptr<Shader>& pTessellationEvaluationShader,
			const std::shared_ptr<Shader>& pGeometryShader,
			const std::shared_ptr<Shader>& pFragmentShader)
			: Pipeline(pDevice),
			pRenderTarget(pRenderTarget),
			pVertexShader(pVertexShader),
			pFragmentShader(pFragmentShader),
			pTessellationControlShader(pTessellationControlShader),
			pTessellationEvaluationShader(pTessellationEvaluationShader),
			pGeometryShader(pGeometryShader)
		{
			if (!pRenderTarget)
				FLINT_THROW_INVALID_ARGUMENT("Render target pointer should not be null!");

			if (pVertexShader == nullptr)
				FLINT_THROW_INVALID_ARGUMENT("Vertex shader pointer should not be null!");

			if (pFragmentShader == nullptr)
				FLINT_THROW_INVALID_ARGUMENT("Fragment shader pointer should not be null!");
		}

	protected:
		std::shared_ptr<RenderTarget> pRenderTarget = nullptr;

		std::shared_ptr<Shader> pVertexShader = nullptr;
		std::shared_ptr<Shader> pFragmentShader = nullptr;
		std::shared_ptr<Shader> pTessellationControlShader = nullptr;
		std::shared_ptr<Shader> pTessellationEvaluationShader = nullptr;
		std::shared_ptr<Shader> pGeometryShader = nullptr;
	};
}