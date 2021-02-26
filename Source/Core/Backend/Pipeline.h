// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.h"

namespace Flint
{
	namespace Backend
	{
		class Buffer;
		class Pipeline;

		typedef std::unordered_map<std::string, Buffer*> UniformBufferContainer;

		/**
		 * Pipeline resource object.
		 * This contains the resources required to submit uniforms to the draw call.
		 */
		class PipelineResource : public BackendObject {
		public:
			PipelineResource() {}

			/**
			 * Initialize the pipeline resource object.
			 *
			 * @param pPipeline: The pipeline pointer which the resource is bound to.
			 */
			virtual void Initialize(Pipeline* pPipeline) = 0;

			/**
			 * Terminate the pipeline resource.
			 */
			virtual void Terminate() = 0;

			/**
			 * Register uniform buffers to the resource object.
			 * All the uniform buffers are to be registered ONCE.
			 *
			 * @param uniformBuffers: The uniform buffers to be registered.
			 */
			virtual void RegisterUniformBuffers(const UniformBufferContainer& uniformBuffers) = 0;

		public:
			/**
			 * Bind the resources to the draw call.
			 * 
			 * @param commandBuffer: The command buffer to be bound to.
			 */
			virtual void Bind(const CommandBuffer& commandBuffer) = 0;

		protected:
			Pipeline* pPipeline = nullptr;
		};

		/**
		 * Pipeline object.
		 * This is the base class for all the pipelines created by Flint.
		 *
		 * Pipelines are made using a render target and shader digests. These pipelines are of 3 types,
		 * 1. Graphics pipeline.
		 * 2. Compute pipeline.
		 * 3. Ray Tracing pipeline.
		 */
		class Pipeline : public BackendObject {
			friend PipelineResource;
		public:
			Pipeline() {}

			/**
			 * Terminate the pipeline.
			 */
			virtual void Terminate() = 0;

			/**
			 * Bind the pipeline to a command buffer.
			 *
			 * @param commandBuffer: The command buffer to be bound to.
			 */
			virtual void Bind(const CommandBuffer& commandBuffer) = 0;

			/**
			 * Unbind the pipeline from a command buffer.
			 *
			 * @param commandBuffer: The command buffer to be unbound from.
			 */
			virtual void UnBind(const CommandBuffer& commandBuffer) = 0;

		public:
			/**
			 * Create uniform buffers which can be bound to the pipeline instance.
			 *
			 * @return The created uniform buffer container.
			 */
			UniformBufferContainer CreateUniformBuffers() const;

			/**
			 * Destroy created uniform buffers.
			 *
			 * @param uniformBuffers: The uniform buffers to be destroyed.
			 */
			void DestroyUniformBuffers(UniformBufferContainer& uniformBuffers) const;

		public:
			/**
			 * Create pipeline resource object.
			 *
			 * @return The created pipeline resource object pointer.
			 */
			virtual PipelineResource* CreatePipelineResource() = 0;

		public:
			RenderTarget* GetRenderTarget() const { return pRenderTarget; }

			std::unordered_map<String, UniformLayout>& GetUniformLayouts() { return mUniformLayouts; }
			const std::unordered_map<String, UniformLayout> GetUniformLayouts() const { return mUniformLayouts; }

		protected:
			/**
			 * Resolve the uniform layouts used by the pipeline.
			 *
			 * @param shaderDigest: The shader digests that contain the uniform data.
			 */
			void ResolveUniformLayouts(const std::vector<ShaderDigest>& shaderDigests);

		protected:
			std::unordered_map<String, UniformLayout> mUniformLayouts;
			RenderTarget* pRenderTarget = nullptr;
		};

		/**
		 * Compute Pipeline object.
		 */
		class ComputePipeline : public Pipeline {
		public:
			ComputePipeline() {}
		};

		/**
		 * Ray Tracing Pipeline object.
		 */
		class RayTracingPipeline : public Pipeline {
		public:
			RayTracingPipeline() {}
		};
	}
}