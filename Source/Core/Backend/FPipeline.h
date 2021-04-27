// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDevice.h"
#include "FShaderDigest.h"
#include "FDynamicStateContainer.h"

namespace Flint
{
	class FRenderTarget;
	class FPipeline;

	class FBuffer;
	using UniformBufferContainer = std::unordered_map<std::string, std::shared_ptr<FBuffer>>;

	class FImage;
	struct SamplerSpecification;
	using UniformImageContainer = std::unordered_map<std::string, std::pair<std::shared_ptr<FImage>, SamplerSpecification>>;

	/**
	 * Pipeline resource object.
	 * This contains the resources required to submit uniforms and images to the draw call.
	 */
	class FPipelineResource : public BackendObject {

		/**
		 * Draw data structure.
		 * This structure contains the final information to be drawn.
		 */
		struct DrawData {
			DrawData() = default;
			DrawData(UI64 vertexCount, UI64 vertexOffset, UI64 indexCount, UI64 indexOffset, DynamicStateContainer dynamicStates)
				: mVertexCount(vertexCount), mVertexOffset(vertexOffset), mIndexCount(indexCount), mIndexOffset(indexOffset), mDynamicStates(dynamicStates) {}

			DynamicStateContainer mDynamicStates = {};

			UI64 mVertexCount = 0;
			UI64 mVertexOffset = 0;

			UI64 mIndexCount = 0;
			UI64 mIndexOffset = 0;
		};

	public:
		FPipelineResource(std::shared_ptr<FPipeline> pPipeline) : pPipeline(pPipeline) {}

		/**
		 * Register uniform buffers to the resource object.
		 * All the uniform buffers are to be registered ONCE.
		 *
		 * @param uniformBuffers: The uniform buffers to be registered.
		 */
		virtual void RegisterUniformBuffers(const UniformBufferContainer& uniformBuffers) = 0;

		/**
		 * Register uniform images to the resource object.
		 * All the uniform images are to be registered ONCE.
		 *
		 * @param unformImages: The uniform images to be registered.
		 */
		virtual void RegisterUniformImages(const UniformImageContainer& unformImages) = 0;

		/**
		 * Get the pipeline which the resource is bound to.
		 *
		 * @return The pipeline pointer.
		 */
		std::shared_ptr<FPipeline> GetPipeline() const { return pPipeline; }

		/**
		 * Add draw data to the pipeline resource.
		 *
		 * @param vertexCount: The number of vertexes to draw.
		 * @param vertexOffset: The vertex offset in the buffer.
		 * @param indexCount: The number of indexes to draw.
		 * @param indexOffset: The index offset in the buffer.
		 * @param dynamicStates: The dynamic states of the pipeline to be drawn.
		 * @return Draw data ID.
		 */
		UI64 AddDrawData(UI64 vertexCount, UI64 vertexOffset, UI64 indexCount, UI64 indexOffset, DynamicStateContainer dynamicStates)
		{
			mDrawData[mIndex] = DrawData(vertexCount, vertexOffset, indexCount, indexOffset, dynamicStates);
			return mIndex++;
		}

		/**
		 * Get draw data from the resource.
		 *
		 * @return The draw data map.
		 */
		std::unordered_map<UI64, DrawData>& GetDrawData() { return mDrawData; }

		/**
		 * Get draw data from the resource.
		 *
		 * @return The draw data map.
		 */
		const std::unordered_map<UI64, DrawData> GetDrawData() const { return mDrawData; }

		/**
		 * Get the draw data index counter.
		 *
		 * @return The index.
		 */
		UI64 GetDrawDataIndex() const { return mIndex; }

	protected:
		std::unordered_map<UI64, DrawData> mDrawData;
		UI64 mIndex = 0;

		std::shared_ptr<FPipeline> pPipeline = nullptr;
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
	class FPipeline : public BackendObject {
	public:
		FPipeline(std::shared_ptr<FRenderTarget> pRenderTarget, std::vector<ShaderDigest> digest) {}
		virtual ~FPipeline() {}

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
		UI64 AddDrawResource(FPipelineResource* pResource) { mDrawResources[mIndex] = pResource; return mIndex++; }

		/**
		 * Get draw resource pointer using the index.
		 *
		 * @param ID: The resource ID.
		 * @return The resource pointer.
		 */
		FPipelineResource* GetDrawResource(UI64 ID) const { return mDrawResources.at(ID); }

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
		virtual std::shared_ptr<FPipelineResource> CreatePipelineResource() = 0;

	public:
		/**
		 * Get the render target which the pipeline is bound to.
		 *
		 * @return The render target pointer.
		 */
		std::shared_ptr<FRenderTarget> GetRenderTarget() const { return pRenderTarget; }

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
		std::unordered_map<UI64, FPipelineResource*>& GetDrawResources() { return mDrawResources; }

		/**
		 * Get draw resource of the pipeline.
		 *
		 * @return Draw resource map.
		 */
		const std::unordered_map<UI64, FPipelineResource*> GetDrawResources() const { return mDrawResources; }

		/**
		 * Get the draw resource index counter.
		 *
		 * @return The index.
		 */
		UI64 GetDrawResourcesIndex() const { return mIndex; }

	protected:
		/**
		 * Resolve the uniform layouts used by the pipeline.
		 *
		 * @param shaderDigest: The shader digests that contain the uniform data.
		 */
		void ResolveUniformLayouts(const std::vector<ShaderDigest>& shaderDigests);

	protected:
		std::unordered_map<String, UniformLayout> mUniformLayouts;
		std::vector<ShaderDigest> mDigests;

		std::unordered_map<UI64, FPipelineResource*> mDrawResources;

		std::shared_ptr<FRenderTarget> pRenderTarget = nullptr;

		UI64 mIndex = 0;
	};

	/**
	 * Compute Pipeline object.
	 */
	class ComputePipeline : public FPipeline {
	public:
		ComputePipeline(std::shared_ptr<FRenderTarget> pRenderTarget, std::vector<ShaderDigest> digest) : FPipeline(pRenderTarget, digest) {}
	};

	/**
	 * Ray Tracing Pipeline object.
	 */
	class RayTracingPipeline : public FPipeline {
	public:
		RayTracingPipeline(std::shared_ptr<FRenderTarget> pRenderTarget, std::vector<ShaderDigest> digest) : FPipeline(pRenderTarget, digest) {}
	};
}