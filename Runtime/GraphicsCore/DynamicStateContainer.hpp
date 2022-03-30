// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Dynamic sate object.
	 * This is the base class for all the dynamic states supported.
	 */
	struct DynamicStateObject
	{
		explicit DynamicStateObject(DynamicStateFlags flag) : m_Flag(flag) {}
		virtual ~DynamicStateObject() {}

		/**
		 * Get the state flags of the object.
		 *
		 * @return The dynamic state flag.
		 */
		DynamicStateFlags GetFlag() const { return m_Flag; }

		DynamicStateFlags m_Flag = DynamicStateFlags::Undefined;
	};

	/**
	 * Viewport structure.
	 * Viewport is the area that is being rendered to the screen. Think of it as a window and the content within the boundary will be displayed.
	 */
	struct ViewPort final : public DynamicStateObject
	{
		ViewPort() : DynamicStateObject(DynamicStateFlags::ViewPort) {}
		explicit ViewPort(const FExtent2D<float>& extent, const FExtent2D<float>& offset, const FExtent2D<float>& depth)
			: DynamicStateObject(DynamicStateFlags::ViewPort), m_Extent(extent), m_Offset(offset), m_Depth(depth) {}

		FExtent2D<float> m_Extent = {};
		FExtent2D<float> m_Offset = {};
		FExtent2D<float> m_Depth = {};	// { Min, Max }
	};

	/**
	 * Scissor structure.
	 */
	struct Scissor final : public DynamicStateObject
	{
		Scissor() : DynamicStateObject(DynamicStateFlags::Scissor) {}
		explicit Scissor(const FBox2D& extent, const FBox2D& offset) : DynamicStateObject(DynamicStateFlags::Scissor), m_Extent(extent), m_Offset(offset) {}

		FBox2D m_Extent = {};
		FBox2D m_Offset = {};
	};

	/**
	 * Line width structure.
	 * This defines the rendering line width.
	 */
	struct LineWidth final : public DynamicStateObject
	{
		LineWidth() : DynamicStateObject(DynamicStateFlags::LineWidth) {}
		explicit LineWidth(const float width) : DynamicStateObject(DynamicStateFlags::LineWidth), m_LineWidth(width) {}

		float m_LineWidth = 1.0f;
	};

	/**
	 * Depth bias structure.
	 * Set the depth bias.
	 */
	struct DepthBias final : public DynamicStateObject
	{
		DepthBias() : DynamicStateObject(DynamicStateFlags::DepthBias) {}
		explicit DepthBias(const float biasFactor, const float clampFactor, const float slopeFactor)
			: DynamicStateObject(DynamicStateFlags::DepthBias), m_DepthBiasFactor(biasFactor),
			m_DepthClampFactor(clampFactor), m_DepthSlopeFactor(slopeFactor) {}

		float m_DepthBiasFactor = 0.0f;
		float m_DepthClampFactor = 0.0f;
		float m_DepthSlopeFactor = 0.0f;
	};

	/**
	 * Blend constants structure.
	 * Set the pipeline blend constants.
	 */
	struct BlendConstants final : public DynamicStateObject
	{
		BlendConstants() : DynamicStateObject(DynamicStateFlags::BlendConstants) {}
		explicit BlendConstants(const float(&constants)[4]) : DynamicStateObject(DynamicStateFlags::BlendConstants) { std::copy(constants, constants + 4, m_Constants); }

		float m_Constants[4] = {};
	};

	/**
	 * Depth bounds struct.
	 * Set pipeline depth bounds.
	 */
	struct DepthBounds final : public DynamicStateObject
	{
		DepthBounds() : DynamicStateObject(DynamicStateFlags::DepthBounds) {}
		explicit DepthBounds(const FExtent2D<float>& bounds) : DynamicStateObject(DynamicStateFlags::DepthBounds), m_Bounds(bounds) {}

		FExtent2D<float> m_Bounds = {};	// { Min, Max }
	};

	/**
	 * Constant data struct.
	 * This struct is used to pass data to constant blocks.
	 */
	struct ConstantData
	{
		ConstantData() = default;
		explicit ConstantData(const void* pData, const uint64_t size, const uint64_t offset) : pData(pData), m_Size(size), m_Offset(offset) {}

		/**
		 * Check if the constant block is null.
		 *
		 * @return The boolean value.
		 */
		constexpr bool IsNull() const { return pData == nullptr; }

		const void* pData = nullptr;
		uint64_t m_Size = 0;
		uint64_t m_Offset = 0;
	};

	/**
	 * Flint dynamic state container.
	 * This object contains the dynamic states which are to be passed to the pipeline at draw time.
	 */
	struct DynamicStateContainer
	{
		DynamicStateContainer() = default;

		/**
		 * Set a viewport to the container.
		 *
		 * @param extent The extent of the viewport.
		 * @param depth The viewport depth.
		 * @param offset The viewport offset.
		 */
		void SetViewPort(const FExtent2D<float>& extent, const FExtent2D<float>& depth, const FExtent2D<float>& offset);

		/**
		 * Set a scissor to the container.
		 *
		 * @param extent The scissor extent.
		 * @param offset The scissor offset.
		 */
		void SetScissor(const FBox2D& extent, const FBox2D& offset);

		/**
		 * Set a line width to the container.
		 *
		 * @param width The line width in float.
		 */
		void SetLineWidth(const float width);

		/**
		 * Set depth bias to the container.
		 *
		 * @param biasFactor The depth bias factor.
		 * @param clampFactor The depth bias clamp factor.
		 * @param slopeFacror The depth bias slope factor.
		 */
		void SetDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor);

		/**
		 * Set blend constants to the container.
		 *
		 * @param constants The static float[4] constants.
		 */
		void SetBlendConstants(const float(&constants)[4]);

		/**
		 * Set depth bounds to the container.
		 *
		 * @param bounds The depth bounds.
		 */
		void SetDepthBounds(const FExtent2D<float>& bounds);

		/**
		 * Set constant data which is to be submitted to shaders.
		 *
		 * @param shaderType The shader type to which the data is submitted to.
		 * @param pData The data pointer.
		 * @param size The size to be passed.
		 * @param offset The offset of the data. Default is 0.
		 */
		void SetConstantData(const ShaderType shaderType, const void* pData, const uint64_t size, const uint64_t offset = 0);

	public:
		ViewPort m_ViewPort = {};
		Scissor m_Scissor = {};
		LineWidth m_LineWidth = {};
		DepthBias m_DepthBias = {};
		BlendConstants m_BlendConstants = {};
		DepthBounds m_DepthBounds = {};

		DynamicStateFlags m_Flags = DynamicStateFlags::Undefined;

		ConstantData m_ConstantBlocks[10] = {};
	};
}