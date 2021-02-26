// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.h"
#include "Core/Macros/Global.h"

namespace Flint
{
	namespace Backend
	{
		enum class PrimitiveTopology : UI8 {
			POINT_LIST,
			LINE_LIST,
			LINE_STRIP,
			TRIANGLE_LIST,
			TRIANGLE_STRIP,
			TRIANGLE_FAN,
			LINE_LIST_WITH_ADJACENCY,
			LINE_STRIP_WITH_ADJACENCY,
			TRIANGLE_LIST_WITH_ADJACENCY,
			TRIANGLE_STRIP_WITH_ADJACENCY,
			PATCH_LIST,
		};

		enum class CullMode : UI8 {
			NONE,
			FRONT,
			BACK,
			FRONT_AND_BACK
		};

		enum class FrontFace : UI8 {
			COUNTER_CLOCKWISE,
			CLOCKWISE,
		};

		enum class PolygonMode : UI8 {
			FILL,
			LINE,
			POINT,
		};

		enum class ColorBlendLogic : UI8 {
			CLEAR,
			AND,
			AND_REVERSE,
			COPY,
			AND_INVERTED,
			NO_OP,
			XOR,
			OR,
			NOR,
			EQUIVALENT,
			INVERT,
			OR_REVERSE,
			COPY_INVERTED,
			OR_INVERTED,
			NAND,
			SET,
		};

		enum class DepthCompareLogic : UI8 {
			NEVER,
			LESS,
			EQUAL,
			LESS_OR_EQUAL,
			GREATER,
			NOT_EQUAL,
			GREATER_OR_EQUAL,
			ALWAYS,
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

		struct GraphicsPipelineSpecification {
			float mColorBlendConstants[4] = {
				CREATE_COLOR_256(255), CREATE_COLOR_256(255),
				CREATE_COLOR_256(255), CREATE_COLOR_256(255)
			};
			float mDepthBiasFactor = 0.0f;
			float mDepthConstantFactor = 0.0f;
			float mDepthSlopeFactor = 0.0f;
			float mRasterizerLineWidth = 0.0f;
			float mMinSampleShading = 1.0f;

			PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
			CullMode mCullMode = CullMode::BACK;
			FrontFace mFrontFace = FrontFace::COUNTER_CLOCKWISE;
			PolygonMode mPolygonMode = PolygonMode::FILL;
			ColorBlendLogic mColorBlendLogic = ColorBlendLogic::CLEAR;
			DepthCompareLogic mDepthCompareLogic = DepthCompareLogic::LESS_OR_EQUAL;
			DynamicStateFlags mDynamicStateFlags = DynamicStateFlags(0);
			//DynamicStateFlags mDynamicStateFlags = DynamicStateFlags::VIEWPORT | DynamicStateFlags::SCISSOR;

			bool bEnablePrimitiveRestart = false;
			bool bEnableDepthBias = false;
			bool bEnableDepthClamp = false;
			bool bEnableRasterizerDiscard = false;
			bool bEnableAlphaCoverage = true;
			bool bEnableAlphaToOne = true;
			bool bEnableSampleShading = true;
			bool bEnableColorBlendLogic = true;
			bool bEnableColorBlend = false;
			bool bEnableDepthTest = false;
			bool bEnableDepthWrite = false;
		};

		/**
		 * Graphics Pipeline object.
		 */
		class GraphicsPipeline : public Pipeline {
		public:
			GraphicsPipeline() {}

			/**
			 * Initialize the pipeline.
			 *
			 * @param pRenderTarget: The render target that the pipeline is bound to.
			 * @param shaderDigest: The shader digests to use.
			 */
			virtual void Initialize(RenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigests, const GraphicsPipelineSpecification& spec) = 0;

		protected:
			GraphicsPipelineSpecification mSpec = {};
		};

		struct DynamicStateContainer {
			struct DynamicStateObject {
				DynamicStateObject(DynamicStateFlags flag) : mFlag(flag) {}

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
				Scissor(const Vector2& extent) : DynamicStateObject(DynamicStateFlags::VIEWPORT), mExtent(extent) {}

				Vector2 mExtent = Vector2::ZeroAll;
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
			void AddScissor(const Vector2& extent);
			void AddLineWidth(const float width);
			void AddDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor);
			void AddBlendConstants(const float(&constants)[4]);
			void AddDepthBounds(const Vector2& bounds);

			DynamicStateContainer& operator=(const DynamicStateContainer& other);
			DynamicStateContainer& operator=(DynamicStateContainer&& other) noexcept;

			std::vector<DynamicStateObject*> pDynamicStates;
		};
	}

	namespace GraphicsPipelinePresets
	{
		/**
		 * Create default specification.
		 * This specification can be used with the following,
		 * * Shaders containing 1 input attribute (Vector3 - Position).
		 * * Uniform buffer for MVP (Model View Projection).
		 * * 2 Shader modules (Vertex and Fragment).
		 */
		Backend::GraphicsPipelineSpecification CreateDefaultSpec();
	}
}