// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "ShaderDigest.h"
#include "Core\Maths\Vector\Vector2.h"

namespace Flint
{
	namespace Backend
	{
		template<class Derived, class DeviceType>
		class Buffer;

		template<class DerivedType, class DeviceType, class RenderTargetType, class BufferType, class ResourceType>
		class Pipeline;

		template<class BufferType>
		using UniformBufferContainer = std::unordered_map<std::string, BufferType>;

		/**
		 * Pipeline resource object.
		 * This contains the resources required to submit uniforms to the draw call.
		 */
		template<class TDerived, class TPipeline, class TDevice, class TBuffer>
		class PipelineResource : public BackendObject {
		public:
			using DerivedType = TDerived;
			using PipelineType = TPipeline;
			using DeviceType = TDevice;
			using BufferType = TBuffer;

		public:
			PipelineResource() {}

			/**
			 * Initialize the pipeline resource object.
			 *
			 * @param pPipeline: The pipeline pointer which the resource is bound to.
			 */
			virtual void Initialize(PipelineType* pPipeline) = 0;

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

			PipelineType* GetPipeline() const { return pPipeline; }

		protected:
			PipelineType* pPipeline = nullptr;
		};

		struct DynamicStateContainer;

		/**
		 * Pipeline object.
		 * This is the base class for all the pipelines created by Flint.
		 *
		 * Pipelines are made using a render target and shader digests. These pipelines are of 3 types,
		 * 1. Graphics pipeline.
		 * 2. Compute pipeline.
		 * 3. Ray Tracing pipeline.
		 */
		template<class TDerived, class TDevice, class TRenderTarget, class TBuffer, class TResource>
		class Pipeline : public BackendObject {

		public:
			using DerivedType = TDerived;
			using DeviceType = TDevice;
			using RenderTargetType = TRenderTarget;
			using BufferType = TBuffer;
			using ResourceType = TResource;

			friend PipelineResource<ResourceType, DerivedType, DeviceType, BufferType>;

			struct DrawData {
				DrawData() = default;
				DrawData(UI64 vertexCount, UI64 vertexOffset, UI64 indexCount, UI64 indexOffset, ResourceType* pResource, DynamicStateContainer dynamicStates)
					: mVertexCount(vertexCount), mVertexOffset(vertexOffset), mIndexCount(indexCount), mIndexOffset(indexOffset), pPipelineResource(pResource), mDynamicStates(dynamicStates) {}

				DynamicStateContainer mDynamicStates = {};

				ResourceType* pPipelineResource = nullptr;

				UI64 mVertexCount = 0;
				UI64 mVertexOffset = 0;

				UI64 mIndexCount = 0;
				UI64 mIndexOffset = 0;
			};

			struct DrawEntry {
				DrawEntry() = default;
				DrawEntry(BufferType* pVertexBuffer, BufferType* pIndexBuffer, const std::vector<DrawData>& drawData)
					: pVertexBuffer(pVertexBuffer), pIndexBuffer(pIndexBuffer), mDrawData(drawData) {}

				BufferType* pVertexBuffer = nullptr;
				BufferType* pIndexBuffer = nullptr;

				std::vector<DrawData> mDrawData = {};
			};


		public:
			Pipeline() {}

			UI64 AddStaticDrawEntry(BufferType* pVertexBuffer, BufferType* pIndexBuffer, const std::vector<DrawData>& drawData = {})
			{
				INSERT_INTO_VECTOR(mStaticDrawEntries, DrawEntry(pVertexBuffer, pIndexBuffer, drawData));
				return mStaticDrawEntries.size() - 1;
			}

			UI64 AddStaticDrawData(UI64 entryID, UI64 vertexCount, UI64 vertexOffset, UI64 indexCount, UI64 indexOffset, ResourceType* pResource, DynamicStateContainer dynamicStates)
			{
				INSERT_INTO_VECTOR(mStaticDrawEntries[entryID].mDrawData, DrawData(vertexCount, vertexOffset, indexCount, indexOffset, pResource, dynamicStates));
				return mStaticDrawEntries[entryID].mDrawData.size() - 1;
			}

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

		public:
			/**
			 * Create uniform buffers which can be bound to the pipeline instance.
			 *
			 * @return The created uniform buffer container.
			 */
			UniformBufferContainer<BufferType> CreateUniformBuffers() const
			{
				DeviceType* pDevice = pRenderTarget->GetDevice();

				Backend::UniformBufferContainer<BufferType> container;
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

			/**
			 * Destroy created uniform buffers.
			 *
			 * @param uniformBuffers: The uniform buffers to be destroyed.
			 */
			void DestroyUniformBuffers(UniformBufferContainer<BufferType>& uniformBuffers) const
			{
				for (auto bufferPair : uniformBuffers)
					bufferPair.second.Terminate();

				uniformBuffers.clear();
			}

		public:
			/**
			 * Create pipeline resource object.
			 *
			 * @return The created pipeline resource object pointer.
			 */
			virtual ResourceType CreatePipelineResource() = 0;

		public:
			RenderTargetType* GetRenderTarget() const { return pRenderTarget; }

			std::unordered_map<String, UniformLayout>& GetUniformLayouts() { return mUniformLayouts; }
			const std::unordered_map<String, UniformLayout> GetUniformLayouts() const { return mUniformLayouts; }

			std::vector<DrawEntry>& GetStaticDrawEntries() { return mStaticDrawEntries; }
			const std::vector<DrawEntry> GetStaticDrawEntries() const { return mStaticDrawEntries; }

			std::vector<DrawEntry>& GetStaticDynamicDrawEntries() { return mDynamicDrawEntries; }
			const std::vector<DrawEntry> GetStaticDynamicDrawEntries() const { return mDynamicDrawEntries; }

		protected:
			/**
			 * Resolve the uniform layouts used by the pipeline.
			 *
			 * @param shaderDigest: The shader digests that contain the uniform data.
			 */
			void ResolveUniformLayouts(const std::vector<ShaderDigest>& shaderDigests)
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

		protected:
			std::unordered_map<String, UniformLayout> mUniformLayouts;
			std::vector<ShaderDigest> mDigests;

			std::vector<DrawEntry> mStaticDrawEntries;
			std::vector<DrawEntry> mDynamicDrawEntries;
			RenderTargetType* pRenderTarget = nullptr;
		};

		enum class DynamicStateFlags : UI8 {
			UNDEFINED,
			VIEWPORT = BIT_SHIFT(0),
			SCISSOR = BIT_SHIFT(1),
			LINE_WIDTH = BIT_SHIFT(2),
			DEPTH_BIAS = BIT_SHIFT(3),
			BLEND_CONSTANTS = BIT_SHIFT(4),
			DEPTH_BOUNDS = BIT_SHIFT(5),
		};

		constexpr DynamicStateFlags operator|(const DynamicStateFlags& lhs, const DynamicStateFlags& rhs)
		{
			return static_cast<DynamicStateFlags>(static_cast<UI8>(lhs) | static_cast<UI8>(rhs));
		}

		constexpr bool operator&(const DynamicStateFlags& lhs, const DynamicStateFlags& rhs)
		{
			return static_cast<UI8>(lhs) & static_cast<UI8>(rhs);
		}

		struct DynamicStateContainer {
			struct DynamicStateObject {
				DynamicStateObject(DynamicStateFlags flag) : mFlag(flag) {}
				virtual ~DynamicStateObject() {}

				DynamicStateFlags GetFlag() const { return mFlag; }

				DynamicStateFlags mFlag = DynamicStateFlags::UNDEFINED;
			};

			struct ViewPort : public DynamicStateObject {
				ViewPort() : DynamicStateObject(DynamicStateFlags::VIEWPORT) {}
				ViewPort(const Vector2& extent, const Vector2& offset, const Vector2& depth)
					: DynamicStateObject(DynamicStateFlags::VIEWPORT), mExtent(extent), mOffset(offset), mDepth(depth) {}

				Vector2 mExtent = Vector2::ZeroAll;
				Vector2 mOffset = Vector2::ZeroAll;
				Vector2 mDepth = Vector2::ZeroAll;	// { Min, Max }
			};

			struct Scissor : public DynamicStateObject {
				Scissor() : DynamicStateObject(DynamicStateFlags::SCISSOR) {}
				Scissor(const Vector2& extent, const Vector2& offset) : DynamicStateObject(DynamicStateFlags::SCISSOR), mExtent(extent), mOffset(offset) {}

				Vector2 mExtent = Vector2::ZeroAll;
				Vector2 mOffset = Vector2::ZeroAll;
			};

			struct LineWidth : public DynamicStateObject {
				LineWidth() : DynamicStateObject(DynamicStateFlags::LINE_WIDTH) {}
				LineWidth(const float width) : DynamicStateObject(DynamicStateFlags::LINE_WIDTH), mLineWidth(width) {}

				float mLineWidth = 1.0f;
			};

			struct DepthBias : public DynamicStateObject {
				DepthBias() : DynamicStateObject(DynamicStateFlags::DEPTH_BIAS) {}
				DepthBias(const float biasFactor, const float clampFactor, const float slopeFactor)
					: DynamicStateObject(DynamicStateFlags::DEPTH_BIAS), mDepthBiasFactor(biasFactor),
					mDepthClampFactor(clampFactor), mDepthSlopeFactor(slopeFactor) {}

				float mDepthBiasFactor = 0.0f;
				float mDepthClampFactor = 0.0f;
				float mDepthSlopeFactor = 0.0f;
			};

			struct BlendConstants : public DynamicStateObject {
				BlendConstants() : DynamicStateObject(DynamicStateFlags::BLEND_CONSTANTS) {}
				BlendConstants(const float(&constants)[4]) : DynamicStateObject(DynamicStateFlags::BLEND_CONSTANTS)
				{
					std::copy(constants, constants + 4, mConstants);
				}

				float mConstants[4] = {};
			};

			struct DepthBounds : public DynamicStateObject {
				DepthBounds() : DynamicStateObject(DynamicStateFlags::DEPTH_BOUNDS) {}
				DepthBounds(const Vector2& bounds) : DynamicStateObject(DynamicStateFlags::DEPTH_BOUNDS), mBounds(bounds) {}

				Vector2 mBounds = Vector2::ZeroAll;	// { Min, Max }
			};

		public:
			DynamicStateContainer() {}
			DynamicStateContainer(const DynamicStateContainer& other);
			DynamicStateContainer(DynamicStateContainer&& other) noexcept;
			~DynamicStateContainer();

			void AddViewPort(const Vector2& extent, const Vector2& depth, const Vector2& offset);
			void AddScissor(const Vector2& extent, const Vector2& offset);
			void AddLineWidth(const float width);
			void AddDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor);
			void AddBlendConstants(const float(&constants)[4]);
			void AddDepthBounds(const Vector2& bounds);

			DynamicStateContainer& operator=(const DynamicStateContainer& other);
			DynamicStateContainer& operator=(DynamicStateContainer&& other) noexcept;

			std::vector<std::shared_ptr<DynamicStateObject>> pDynamicStates;
		};

		/**
		 * Compute Pipeline object.
		 */
		template<class DerivedType, class DeviceType, class RenderTargetType, class BufferType, class ResourceType>
		class ComputePipeline : public Pipeline<DerivedType, DeviceType, RenderTargetType, BufferType, ResourceType> {
		public:
			ComputePipeline() {}
		};

		/**
		 * Ray Tracing Pipeline object.
		 */
		template<class DerivedType, class DeviceType, class RenderTargetType, class BufferType, class ResourceType>
		class RayTracingPipeline : public Pipeline<DerivedType, DeviceType, RenderTargetType, BufferType, ResourceType> {
		public:
			RayTracingPipeline() {}
		};
	}
}