// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBufferManager.h"
#include "ShaderDigest.h"
#include "Core/Macros/Global.h"

namespace Flint
{
	namespace Backend
	{
		class Buffer;
		class Pipeline;
		class RenderTarget;

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
			 * @param pCommandBuffer: The command buffer to be bound to.
			 */
			virtual void Bind(const std::shared_ptr<CommandBuffer>& pCommandBuffer) = 0;

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
			 * Prepare the pipeline to recreate.
			 * This method must be called before recreating the pipeline.
			 */
			virtual void PrepareToRecreate() = 0;

			/**
			 * Recreate the pipeline if the resources were changed.
			 */
			virtual void Recreate() = 0;

			/**
			 * Bind the pipeline to a command buffer.
			 *
			 * @param pCommandBuffer: The command buffer to be bound to.
			 */
			virtual void Bind(const std::shared_ptr<CommandBuffer>& pCommandBuffer) = 0;

			/**
			 * Unbind the pipeline from a command buffer.
			 *
			 * @param pCommandBuffer: The command buffer to be unbound from.
			 */
			virtual void UnBind(const std::shared_ptr<CommandBuffer>& pCommandBuffer) = 0;

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
			std::vector<ShaderDigest> mDigests;
			RenderTarget* pRenderTarget = nullptr;
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