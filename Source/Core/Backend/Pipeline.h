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
		template<class Derived, class DeviceType>
		class Buffer;

		template<class BufferType>
		using UniformBufferContainer = std::unordered_map<std::string, BufferType>;

		struct SamplerSpecification;

		template<class ImageType>
		using UniformImageContainer = std::unordered_map<std::string, std::pair<ImageType, SamplerSpecification>>;

		/**
		 * Pipeline resource object.
		 * This contains the resources required to submit uniforms and images to the draw call.
		 *
		 * @tparam TDerived: The derived type.
		 * @tparam TPipeline: The pipeline type.
		 * @tparam TBuffer: The buffer type.
		 * @tparam TImage: The image type.
		 */
		template<class TDerived, class TPipeline, class TBuffer, class TImage>
		class PipelineResource : public BackendObject {
		public:
			using DerivedType = TDerived;
			using BufferType = TBuffer;
			using ImageType = TImage;

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
			PipelineResource() {}

			/**
			 * Initialize the pipeline resource object.
			 *
			 * @param pPipeline: The pipeline pointer which the resource is bound to.
			 */
			virtual void Initialize(TPipeline* pPipeline) = 0;

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
			virtual void RegisterUniformBuffers(const UniformBufferContainer<BufferType>& uniformBuffers) = 0;

			/**
			 * Register uniform images to the resource object.
			 * All the uniform images are to be registered ONCE.
			 *
			 * @param unformImages: The uniform images to be registered.
			 */
			virtual void RegisterUniformImages(const UniformImageContainer<ImageType>& unformImages) = 0;

			/**
			 * Get the pipeline which the resource is bound to.
			 *
			 * @return The pipeline pointer.
			 */
			TPipeline* GetPipeline() const { return pPipeline; }

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

			TPipeline* pPipeline = nullptr;
		};

		/**
		 * Pipeline object.
		 * This is the base class for all the pipelines created by Flint.
		 *
		 * Pipelines are made using a render target and shader digests. These pipelines are of 3 types,
		 * 1. Graphics pipeline.
		 * 2. Compute pipeline.
		 * 3. Ray Tracing pipeline.
		 *
		 * @tparam Derived: The derived type.
		 * @tparam TDevice: The device type.
		 * @tparam TBuffer: The buffer type.
		 * @tparam TImage: The image type.
		 * @tparam TResource: The resource type.
		 */
		template<class Derived, class TDevice, class TRenderTarget, class TBuffer, class TImage, class TResource>
		class Pipeline : public BackendObject {
		public:
			using DeviceType = TDevice;
			using BufferType = TBuffer;
			using ImageType = TImage;
			using ResourceType = TResource;
			using RenderTargetType = TRenderTarget;

			friend PipelineResource<ResourceType, Derived, BufferType, ImageType>;

		public:
			Pipeline() {}

			/**
			 * Terminate the pipeline.
			 */
			virtual void Terminate() = 0;

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
			UI64 AddDrawResource(ResourceType* pResource) { mDrawResources[mIndex] = pResource; return mIndex++; }

			/**
			 * Get draw resource pointer using the index.
			 *
			 * @param ID: The resource ID.
			 * @return The resource pointer.
			 */
			ResourceType* GetDrawResource(UI64 ID) const { return mDrawResources.at(ID); }

		public:
			/**
			 * Create uniform buffers which can be bound to the pipeline instance.
			 *
			 * @return The created uniform buffer container.
			 */
			UniformBufferContainer<BufferType> CreateUniformBuffers() const;

			/**
			 * Destroy created uniform buffers.
			 *
			 * @param uniformBuffers: The uniform buffers to be destroyed.
			 */
			void DestroyUniformBuffers(UniformBufferContainer<BufferType>& uniformBuffers) const;

			/**
			 * Create uniform images which can be bound to the pipeline instance.
			 *
			 * @return The created uniform image container.
			 */
			UniformImageContainer<ImageType> CreateUniformImages() const;

			/**
			 * Destroy created uniform images.
			 *
			 * @param uniformBuffers: The uniform images to be destroyed.
			 */
			void DestroyUniformImages(UniformImageContainer<ImageType>& uniformImages) const;

		public:
			/**
			 * Create pipeline resource object.
			 *
			 * @return The created pipeline resource object pointer.
			 */
			virtual ResourceType CreatePipelineResource() = 0;

		public:
			/**
			 * Get the render target which the pipeline is bound to.
			 *
			 * @return The render target pointer.
			 */
			RenderTargetType* GetRenderTarget() const { return pRenderTarget; }

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
			std::unordered_map<UI64, ResourceType*>& GetDrawResources() { return mDrawResources; }

			/**
			 * Get draw resource of the pipeline.
			 *
			 * @return Draw resource map.
			 */
			const std::unordered_map<UI64, ResourceType*> GetDrawResources() const { return mDrawResources; }

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

			std::unordered_map<UI64, ResourceType*> mDrawResources;
			UI64 mIndex = 0;

			RenderTargetType* pRenderTarget = nullptr;
		};

		/**
		 * Compute Pipeline object.
		 */
		template<class DerivedType, class DeviceType, class RenderTargetType, class BufferType, class ImageType, class ResourceType>
		class ComputePipeline : public Pipeline<DerivedType, DeviceType, RenderTargetType, BufferType, ImageType, ResourceType> {
		public:
			ComputePipeline() {}
		};

		/**
		 * Ray Tracing Pipeline object.
		 */
		template<class DerivedType, class DeviceType, class RenderTargetType, class BufferType, class ImageType, class ResourceType>
		class RayTracingPipeline : public Pipeline<DerivedType, DeviceType, RenderTargetType, BufferType, ImageType, ResourceType> {
		public:
			RayTracingPipeline() {}
		};

		template<class Derived, class TDevice, class TRenderTarget, class TBuffer, class TImage, class TResource>
		inline UniformBufferContainer<TBuffer> Pipeline<Derived, TDevice, TRenderTarget, TBuffer, TImage, TResource>::CreateUniformBuffers() const
		{
			DeviceType* pDevice = pRenderTarget->GetDevice();

			UniformBufferContainer<BufferType> container;
			for (auto itr = mUniformLayouts.begin(); itr != mUniformLayouts.end(); itr++)
			{
				if (itr->second.mType == UniformType::UNIFORM_BUFFER || itr->second.mType == UniformType::UNIFORM_BUFFER_DYNAMIC)
				{
					BufferType buffer = {};
					buffer.Initialize(pDevice, itr->second.mSize, Backend::BufferUsage::UNIFORM, Backend::MemoryProfile::TRANSFER_FRIENDLY);
					container[itr->first] = buffer;
				}
			}

			return container;
		}

		template<class Derived, class TDevice, class TRenderTarget, class TBuffer, class TImage, class TResource>
		inline void Pipeline<Derived, TDevice, TRenderTarget, TBuffer, TImage, TResource>::DestroyUniformBuffers(UniformBufferContainer<BufferType>& uniformBuffers) const
		{
			for (auto bufferPair : uniformBuffers)
				bufferPair.second.Terminate();

			uniformBuffers.clear();
		}

		template<class Derived, class TDevice, class TRenderTarget, class TBuffer, class TImage, class TResource>
		inline UniformImageContainer<TImage> Pipeline<Derived, TDevice, TRenderTarget, TBuffer, TImage, TResource>::CreateUniformImages() const
		{
			DeviceType* pDevice = pRenderTarget->GetDevice();

			UniformImageContainer<ImageType> container;
			for (auto itr = mUniformLayouts.begin(); itr != mUniformLayouts.end(); itr++)
			{
				if (itr->second.mType == UniformType::STORAGE_IMAGE ||
					itr->second.mType == UniformType::SAMPLER_2D ||
					itr->second.mType == UniformType::SAMPLER_2D_ARRAY ||
					itr->second.mType == UniformType::SAMPLER_CUBE ||
					itr->second.mType == UniformType::SAMPLER_CUBE_ARRAY)
					container[itr->first] = {};
			}

			return container;
		}

		template<class Derived, class TDevice, class TRenderTarget, class TBuffer, class TImage, class TResource>
		inline void Pipeline<Derived, TDevice, TRenderTarget, TBuffer, TImage, TResource>::DestroyUniformImages(UniformImageContainer<ImageType>& uniformImages) const
		{
			for (auto imagePair : uniformImages)
				imagePair.second.Terminate();

			uniformImages.clear();
		}

		template<class Derived, class TDevice, class TRenderTarget, class TBuffer, class TImage, class TResource>
		inline void Pipeline<Derived, TDevice, TRenderTarget, TBuffer, TImage, TResource>::ResolveUniformLayouts(const std::vector<ShaderDigest>& shaderDigests)
		{
			for (auto itr = shaderDigests.begin(); itr != shaderDigests.end(); itr++)
			{
				for (auto pUniform = itr->mUniforms.begin(); pUniform != itr->mUniforms.end(); pUniform++)
				{
					if (pUniform->mType == UniformType::UNIFORM_BUFFER ||
						pUniform->mType == UniformType::UNIFORM_BUFFER_DYNAMIC ||
						pUniform->mType == UniformType::STORAGE_BUFFER ||
						pUniform->mType == UniformType::STORAGE_BUFFER_DYNAMIC)
						mUniformLayouts[pUniform->mName] = *pUniform;
				}
			}
		}
	}
}