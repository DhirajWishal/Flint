// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Shader.hpp"
#include "RenderTarget.hpp"
#include "DynamicStateContainer.hpp"

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
	 * 5. Fragment (optional).
	 */
	template<class DeviceT, class RenderTargetT, class ResourcePackageT, class ShaderT, class ScreenBoundRenderTargetT, class OffScreenRenderTargetT>
	class GraphicsPipeline : public Pipeline<DeviceT, RenderTargetT, ResourcePackageT>
	{
	public:
		/**
		 * Construct the pipeline using a screen bound render target.
		 *
		 * @param pDevice The device pointer.
		 * @param pipelineName The name of the pipeline.
		 * @param pScreenBoundRenderTarget The screen bound render target pointer.
		 * @param pVertexShader The vertex shader pointer.
		 * @param pTessellationControlShader The tessellation control shader (optional).
		 * @param pTessellationEvaluationShader The tessellation evaluation shader (optional).
		 * @param pGeometryShader The geometry shader (optional).
		 * @param pFragmentShader The fragment shader (optional).
		 * @param specification The pipeline specification.
		 */
		GraphicsPipeline(DeviceT* pDevice, const std::string& pipelineName, const ScreenBoundRenderTargetT* pScreenBoundRenderTarget, std::unique_ptr<ShaderT>&& pVertexShader, std::unique_ptr<ShaderT>&& pTessellationControlShader, std::unique_ptr<ShaderT>&& pTessellationEvaluationShader, std::unique_ptr<ShaderT>&& pGeometryShader, std::unique_ptr<ShaderT>&& pFragmentShader, const GraphicsPipelineSpecification& specification)
			: Pipeline(pDevice, pipelineName)
			, pRenderTarget(pScreenBoundRenderTarget)
			, pVertexShader(std::move(pVertexShader))
			, pTessellationControlShader(std::move(pTessellationControlShader))
			, pTessellationEvaluationShader(std::move(pTessellationEvaluationShader))
			, pGeometryShader(std::move(pGeometryShader))
			, pFragmentShader(std::move(pFragmentShader))
			, m_Specification(specification)
		{
			if (!pScreenBoundRenderTarget)
				throw std::invalid_argument("Render target pointer should not be null!");

			if (pVertexShader == nullptr)
				throw std::invalid_argument("Vertex shader pointer should not be null!");

			if (pFragmentShader == nullptr)
				throw std::invalid_argument("Fragment shader pointer should not be null!");
		}

		/**
		 * Construct the pipeline using an off screen render target.
		 *
		 * @param pDevice The device pointer.
		 * @param pipelineName The name of the pipeline.
		 * @param pOffScreenRenderTarget The off screen render target pointer.
		 * @param pVertexShader The vertex shader pointer.
		 * @param pTessellationControlShader The tessellation control shader (optional).
		 * @param pTessellationEvaluationShader The tessellation evaluation shader (optional).
		 * @param pGeometryShader The geometry shader (optional).
		 * @param pFragmentShader The fragment shader (optional).
		 * @param specification The pipeline specification.
		 */
		GraphicsPipeline(DeviceT* pDevice, const std::string& pipelineName, const OffScreenRenderTargetT* pOffScreenRenderTarget, std::unique_ptr<ShaderT>&& pVertexShader, std::unique_ptr<ShaderT>&& pTessellationControlShader, std::unique_ptr<ShaderT>&& pTessellationEvaluationShader, std::unique_ptr<ShaderT>&& pGeometryShader, std::unique_ptr<ShaderT>&& pFragmentShader, const GraphicsPipelineSpecification& specification)
			: Pipeline(pDevice, pipelineName)
			, pRenderTarget(pOffScreenRenderTarget)
			, pVertexShader(std::move(pVertexShader))
			, pTessellationControlShader(std::move(pTessellationControlShader))
			, pTessellationEvaluationShader(std::move(pTessellationEvaluationShader))
			, pGeometryShader(std::move(pGeometryShader))
			, pFragmentShader(std::move(pFragmentShader))
			, m_Specification(specification)
		{
			if (!pOffScreenRenderTarget)
				throw std::invalid_argument("Render target pointer should not be null!");

			if (pVertexShader == nullptr)
				throw std::invalid_argument("Vertex shader pointer should not be null!");
		}

		/**
		 * Recreate the graphics pipeline.
		 * This is needed once the render target is resized.
		 *
		 * @param pScreenBoundRenderTarget The screen bound render target pointer.
		 */
		virtual void Recreate(ScreenBoundRenderTargetT* pScreenBoundRenderTarget) = 0;

		/**
		 * Get the current graphics specification of the pipeline.
		 *
		 * @return The graphics specification.
		 */
		const GraphicsPipelineSpecification GetSpecification() const { return m_Specification; }

	public:
		/**
		 * Get the vertex shader pointer.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetVertexShader() const { return pVertexShader.get(); }

		/**
		 * Get the fragment shader pointer.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetFragmentShader() const { return pFragmentShader.get(); }

		/**
		 * Get the tessellation control shader pointer.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetTessellationControlShader() const { return pTessellationControlShader.get(); }

		/**
		 * Get the tessellation evaluation shader pointer.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetTessellationEvaluationShader() const { return pTessellationEvaluationShader.get(); }

		/**
		 * Get the geometry shader pointer.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetGeometryShader() const { return pGeometryShader.get(); }

	protected:
		GraphicsPipelineSpecification m_Specification = {};

		const RenderTargetT* pRenderTarget = nullptr;

		std::unique_ptr<ShaderT> pVertexShader = nullptr;
		std::unique_ptr<ShaderT> pFragmentShader = nullptr;
		std::unique_ptr<ShaderT> pTessellationControlShader = nullptr;
		std::unique_ptr<ShaderT> pTessellationEvaluationShader = nullptr;
		std::unique_ptr<ShaderT> pGeometryShader = nullptr;
	};
}