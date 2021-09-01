// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/FObject.hpp"
#include "Core/Error.hpp"

#include <vector>

#include <unordered_map>


namespace Flint
{
	class Device;
	class Image;

	/**
	 * Vertex attribute data type enum.
	 * This enum contains all the supported vertex attribute data types.
	 */
	enum class ShaderAttributeDataType : UI8 {
		BYTE = 1,

		I8 = 1,
		UI8 = 1,
		SI8 = 1,
		I16 = 2,
		UI16 = 2,
		SI16 = 2,
		I32 = 4,
		UI32 = 4,
		SI32 = 4,
		I64 = 8,
		UI64 = 8,
		SI64 = 8,

		FLOAT = 4,
		F64 = 4,
		D64 = 8,
		CHAR = 1,

		PTR = 8,
		VOID_PTR = 8,

		VEC1 = 4,
		VEC2 = 8,
		VEC3 = 12,
		VEC4 = 16,
		UVEC1 = 4,
		UVEC2 = 8,
		UVEC3 = 12,
		UVEC4 = 16,
		SVEC1 = 4,
		SVEC2 = 8,
		SVEC3 = 12,
		SVEC4 = 16,
		BVEC1 = 1,
		BVEC2 = 2,
		BVEC3 = 3,
		BVEC4 = 4,

		MAT2 = 16,
		MAT3 = 36,
		MAT4 = 64,

		BUILT_IN = 255
	};

	/**
	 * Buffer type enum.
	 */
	enum class BufferType : UI8 {
		Undefined,
		Staging,
		Vertex, Index,
		Uniform, Storage
	};

	/**
	 * Buffer memory profile enum.
	 * This enum defines the buffer's memory behavior.
	 *
	 * Automatic will set the default memory profile for a given buffer type.
	 * CPUOnly will set the content to the CPU bound memory (RAM).
	 * DeviceOnly will set data in the device and stagging buffers are required to access them.
	 * TransferFriendly will enable it to be mapped to the local address space.
	 */
	enum class BufferMemoryProfile : UI8 {
		Automatic,
		CPUOnly,
		DeviceOnly,
		TransferFriendly
	};

	/**
	 * Image type enum.
	 */
	enum class ImageType : UI8 {
		OneDimension,
		TwoDimension,
		ThreeDimension,
		CubeMap,

		OneDimensionArray,
		TwoDimensionArray,
		ThreeDimensionArray,
		CubeMapArray,
	};

	/**
	 * Image usage enum.
	 */
	enum class ImageUsage : UI8 {
		Graphics = BIT_SHIFT(0),
		Storage = BIT_SHIFT(1),
		Depth = BIT_SHIFT(2),
		Color = BIT_SHIFT(3)
	};

	/**
	 * Bitwise OR operator for the image usage.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The OR-ed value.
	 */
	constexpr ImageUsage operator|(const ImageUsage& lhs, const ImageUsage& rhs) { return static_cast<ImageUsage>(static_cast<UI8>(lhs) | static_cast<UI8>(rhs)); }

	/**
	 * Bitwise And operator for the image usage.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The And-ed value.
	 */
	constexpr ImageUsage operator&(const ImageUsage& lhs, const ImageUsage& rhs) { return static_cast<ImageUsage>(static_cast<UI8>(lhs) & static_cast<UI8>(rhs)); }

	/**
	 * Pixel format enum.
	 */
	enum class PixelFormat : UI8 {
		Undefined,

		R8_SRGB,
		R8G8_SRGB,
		R8G8B8_SRGB,
		R8G8B8A8_SRGB,

		R8_UNORMAL,
		R8G8_UNORMAL,
		R8G8B8_UNORMAL,
		R8G8B8A8_UNORMAL,

		B8G8R8_SRGB,
		B8G8R8A8_SRGB,

		B8G8R8_UNORMAL,
		B8G8R8A8_UNORMAL,

		R16_SFLOAT,
		R16G16_SFLOAT,
		R16G16B16_SFLOAT,
		R16G16B16A16_SFLOAT,

		R32_SFLOAT,
		R32G32_SFLOAT,
		R32G32B32_SFLOAT,
		R32G32B32A32_SFLOAT,

		D16_SINT,
		D32_SFLOAT,

		S8_UINT,
		D16_UNORMAL_S8_UINT,
		D24_UNORMAL_S8_UINT,
		D32_SFLOAT_S8_UINT,
	};

	/**
	 * Address mode enum.
	 */
	enum class AddressMode : UI8 {
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
		MirrorClampToEdge,
	};

	/**
	 * Border color enum.
	 */
	enum class BorderColor : UI8 {
		TransparentBlackFLOAT,
		TransparentBlackINT,
		OpaqueBlackFLOAT,
		OpaqueBlackINT,
		OpaqueWhiteFLOAT,
		OpaqueWhiteINT,
	};

	/**
	 * Compare operator enum.
	 */
	enum class CompareOperator : UI8 {
		Never,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual,
		Always,
	};

	/**
	 * Image filter enum.
	 */
	enum class ImageFilter : UI8 {
		Nearest,
		Linear,
		CubicImage,
	};

	/**
	 *Image mip map mode enum.
	 */
	enum class ImageMipMapMode : UI8 {
		Nearest,
		Linear,
	};

	/**
	 * Image sampler specification structure.
	 */
	struct ImageSamplerSpecification
	{
		// If set to 0.0f, the maximum supported will be set.
		float mMaxAnisotrophy = 0.0f;

		float mMaxLevelOfDetail = 0.0f;
		float mMinLevelOfDetail = 0.0f;

		float mMipLoadBias = 0.0f;

		AddressMode mAddressModeU = AddressMode::Repeat;
		AddressMode mAddressModeV = AddressMode::Repeat;
		AddressMode mAddressModeW = AddressMode::Repeat;

		BorderColor mBorderColor = BorderColor::OpaqueBlackINT;

		CompareOperator mCompareOperator = CompareOperator::Always;

		ImageFilter mImageMagificationFilter = ImageFilter::Linear;
		ImageFilter mImageMinificationFilter = ImageFilter::Linear;

		ImageMipMapMode mMipMapMode = ImageMipMapMode::Linear;

		bool bEnableAnisotropy = true;
		bool bEnableCompare = false;
		bool bEnableUnnormalizedCoordinates = false;
	};

	/**
	 * Shader code type enum.
	 */
	enum class ShaderType : UI8 {
		Undefined,
		Vertex, TessellationControl, TessellationEvaluation, Geometry, Fragment,
		Compute,
		RayGen, AnyHit, ClosestHit, RayMiss
	};

	/**
	 * Rasterization samples enum.
	 */
	enum class MultiSampleCount : UI8 {
		One = BIT_SHIFT(0),
		Two = BIT_SHIFT(1),
		Four = BIT_SHIFT(2),
		Eight = BIT_SHIFT(3),
		Sixteen = BIT_SHIFT(4),
		ThirtyTwo = BIT_SHIFT(5),
		SixtyFour = BIT_SHIFT(6),
	};

	/**
	 * Rasterization samples bitwise OR operator.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The OR performed samples.
	 */
	constexpr MultiSampleCount operator|(const MultiSampleCount& lhs, const MultiSampleCount& rhs) { return static_cast<MultiSampleCount>(static_cast<UI8>(lhs) | static_cast<UI8>(rhs)); }

	/**
	 * Rasterization samples bitwise And operator.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The And performed samples.
	 */
	constexpr MultiSampleCount operator&(const MultiSampleCount& lhs, const MultiSampleCount& rhs) { return static_cast<MultiSampleCount>(static_cast<UI8>(lhs) & static_cast<UI8>(rhs)); }

	/**
	 * Screen bound render target present mode.
	 */
	enum class SwapChainPresentMode : UI8 {
		Immediate,
		MailBox,
		FIFO,
		RelaxedFIFO,
		SharedDemandRefresh,
		SharedContinuousRefresh,
	};

	/**
	 * Depth clear values structure.
	 */
	struct DepthClearValues
	{
		DepthClearValues() = default;
		DepthClearValues(float depth, UI32 stencil) : mDepth(depth), mStencil(stencil) {}

		float mDepth = 0.0f;
		UI32 mStencil = 0;
	};

	/**
	 * Render target attachment structure.
	 */
	struct RenderTargetAttachment
	{
		RenderTargetAttachment() = default;
		RenderTargetAttachment(const std::shared_ptr<Image>& pImage, const FColor4D& clearColor) : pImage(pImage), mClearColor(clearColor) {}
		RenderTargetAttachment(const std::shared_ptr<Image>& pImage, const DepthClearValues& depthValue) : pImage(pImage), mDepthClearValue(depthValue) {}

		std::shared_ptr<Image> pImage = nullptr;

		FColor4D mClearColor = FColor4D(CREATE_COLOR_256(32.0f), CREATE_COLOR_256(32.0f), CREATE_COLOR_256(32.0f), 1.0f);
		DepthClearValues mDepthClearValue = {};
	};

	/**
	 * Flint device bound object.
	 * This object is the base class for all the resources in Flint which are based on a device.
	 */
	class DeviceBoundObject : public FObject
	{
		friend Device;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 */
		DeviceBoundObject(const std::shared_ptr<Device>& pDevice);

		/**
		 * Get the device of this object.
		 *
		 * @return The device pointer.
		 */
		std::shared_ptr<Device> GetDevice() const { return pDevice; }

		/**
		 * Terminate the device bound object.
		 */
		virtual void Terminate() = 0;

	protected:
		std::shared_ptr<Device> pDevice = nullptr;
	};
}