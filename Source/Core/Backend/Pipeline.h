// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.h"

namespace Flint
{
	namespace Backend
	{
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
			RenderTarget* GetRenderTarget() const { return pRenderTarget; }

		protected:
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