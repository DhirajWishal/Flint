// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/Handle.h"
#include "Core/Maths/Vector/Vector2.h"

namespace Flint
{
	namespace Interface
	{
		FLINT_DEFINE_HANDLE(InstanceHandle);
		FLINT_DEFINE_HANDLE(DisplayHandle);
		FLINT_DEFINE_HANDLE(DeviceHandle);

		FLINT_DEFINE_HANDLE(RenderTargetHandleSB2D);	// Screen Bound 2D
		FLINT_DEFINE_HANDLE(RenderTargetHandleSB3D);	// Screen Bound 3D
		FLINT_DEFINE_HANDLE(RenderTargetHandleSBRT);	// Screen Bound Ray Traced
		FLINT_DEFINE_HANDLE(RenderTargetHandleOS2D);	// Off Screen 2D
		FLINT_DEFINE_HANDLE(RenderTargetHandleOS3D);	// Off Screen 3D
		FLINT_DEFINE_HANDLE(RenderTargetHandleOSRT);	// Off Screen Ray Traced

		FLINT_DEFINE_HANDLE(GraphicsPipelineHandle);
		FLINT_DEFINE_HANDLE(ComputePipelineHandle);
		FLINT_DEFINE_HANDLE(RayTracingPipelineHandle);

		FLINT_DEFINE_HANDLE(ShaderModule);

		FLINT_DEFINE_HANDLE(StaggingBufferHandle);
		FLINT_DEFINE_HANDLE(VertexBufferHandle);
		FLINT_DEFINE_HANDLE(IndexBufferHandle);
		FLINT_DEFINE_HANDLE(UniformBufferHandle);

		FLINT_DEFINE_HANDLE(PipelineResourceHandle);

		/**
		 * Render Target Extent structure.
		 */
		struct RenderTargetExtent {
			RenderTargetExtent() = default;
			RenderTargetExtent(UI32 width, UI32 height) : mWidth(width), mHeight(height) {}
			RenderTargetExtent(const Vector2& vector)
				: mWidth(static_cast<UI32>(vector.x)), mHeight(static_cast<UI32>(vector.y)) {}

			UI32 mWidth = 0;
			UI32 mHeight = 0;
		};
	}

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

	struct GraphicsPipelineSpecification {
		bool bEnablePrimitiveRestart = false;
		PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::LINE_LIST;

		CullMode mCullMode = CullMode::BACK;
		float mDepthBiasFactor = 0.0f;
	};

	typedef std::unordered_map<String, Interface::UniformBufferHandle> UniformBufferStorage;
}

#define FLINT_DEFINE_FUNCTION_POINTER(ret, name, ...)	inline ret (*name)(__VA_ARGS__) = nullptr