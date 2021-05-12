// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "ShaderDigest.h"
#include "DynamicStateContainer.h"

namespace Flint
{
	namespace Backend
	{
		class RenderTarget;
		class PipelineResource;

		class Buffer;
		using UniformBufferContainer = std::unordered_map<std::string, Buffer*>;

		class Image;
		struct SamplerSpecification;
		using UniformImageContainer = std::unordered_map<std::string, std::pair<Image*, SamplerSpecification>>;

		/**
		 * Flint Pipeline object.
		 * This is the base class for all the pipelines created by Flint.
		 *
		 * Pipelines are made using a render target and shader digests. These pipelines are of 3 types,
		 * 1. Graphics pipeline.
		 * 2. Compute pipeline.
		 * 3. Ray Tracing pipeline.
		 */
		class Pipeline : public BackendObject
		{
		public:
			Pipeline(RenderTarget* pRenderTarget, std::vector<ShaderDigest> digest) : pRenderTarget(pRenderTarget), mDigests(digest) {}
			virtual ~Pipeline() {}

			/**
			 * Prepare the pipeline to recreate.
			 * This method must be called before recreating the pipeline.
			 */
			virtual void PrepareToRecreate() = 0;

			/**
			 * Recreate the pipeline if the resources were changed.
			 */
			virtual void Recreate() = 0;

			/**
			 * Add draw resource to the pipeline to be rendered.
			 *
			 * @param pResource: The resource pointer.
			 * @return The draw resource ID.
			 */
			UI64 AddDrawResource(PipelineResource* pResource) { mDrawResources[mIndex] = pResource; return mIndex++; }

			/**
			 * Get draw resource pointer using the index.
			 *
			 * @param ID: The resource ID.
			 * @return The resource pointer.
			 */
			PipelineResource* GetDrawResource(UI64 ID) const { return mDrawResources.at(ID); }

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

			/**
			 * Create uniform images which can be bound to the pipeline instance.
			 *
			 * @return The created uniform image container.
			 */
			UniformImageContainer CreateUniformImages() const;

			/**
			 * Destroy created uniform images.
			 *
			 * @param uniformBuffers: The uniform images to be destroyed.
			 */
			void DestroyUniformImages(UniformImageContainer& uniformImages) const;

		public:
			/**
			 * Create pipeline resource object.
			 *
			 * @return The created pipeline resource object pointer.
			 */
			virtual PipelineResource* CreatePipelineResource() = 0;

		public:
			/**
			 * Get the uniform layouts of the pipeline.
			 *
			 * @return The uniform layout map.
			 */
			std::unordered_map<String, UniformLayout>& GetUniformLayouts() { return mUniformLayouts; }

			/**
			 * Get the uniform layouts of the pipeline.
			 *
			 * @return The uniform layout map.
			 */
			const std::unordered_map<String, UniformLayout> GetUniformLayouts() const { return mUniformLayouts; }

			/**
			 * Get draw resource of the pipeline.
			 *
			 * @return Draw resource map.
			 */
			std::unordered_map<UI64, PipelineResource*>& GetDrawResources() { return mDrawResources; }

			/**
			 * Get draw resource of the pipeline.
			 *
			 * @return Draw resource map.
			 */
			const std::unordered_map<UI64, PipelineResource*> GetDrawResources() const { return mDrawResources; }

			/**
			 * Get the draw resource index counter.
			 *
			 * @return The index.
			 */
			UI64 GetDrawResourcesIndex() const { return mIndex; }

			/**
			 * Get the render target of the pipeline.
			 *
			 * @return The render target pointer.
			 */
			RenderTarget* GetRenderTarget() const { return pRenderTarget; }

		protected:
			/**
			 * Resolve the uniform layouts used by the pipeline.
			 *
			 * @param shaderDigest: The shader digests that contain the uniform data.
			 */
			void ResolveUniformLayouts(const std::vector<ShaderDigest>& shaderDigests);

		protected:
			RenderTarget* pRenderTarget = nullptr;
			std::unordered_map<String, UniformLayout> mUniformLayouts;
			std::vector<ShaderDigest> mDigests;

			std::unordered_map<UI64, PipelineResource*> mDrawResources;

			UI64 mIndex = 0;
		};

		/**
		 * Compute Pipeline object.
		 */
		class ComputePipeline : public Pipeline
		{
		public:
			ComputePipeline(RenderTarget* pRenderTarget, const ShaderDigest& digest) : Pipeline(pRenderTarget, { digest }) {}
		};

		/**
		 * Ray Tracing Pipeline object.
		 */
		class RayTracingPipeline : public Pipeline
		{
		public:
			RayTracingPipeline(RenderTarget* pRenderTarget, std::vector<ShaderDigest> digest) : Pipeline(pRenderTarget, digest) {}
		};
	}
}