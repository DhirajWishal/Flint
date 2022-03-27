// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/FObject.hpp"
#include "Core/Error.hpp"
#include "Core/Hasher.hpp"

#include <vector>
#include <unordered_map>

namespace Flint
{
	/**
	 * Vertex attribute data type enum.
	 * This enum contains all the supported vertex attribute data types.
	 */
	enum class ShaderAttributeDataType : uint8_t
	{
		byte = 1,

		int8_t = 1,
		uint8_t = 1,
		sint8_t = 1,
		int16_t = 2,
		uint16_t = 2,
		sint16_t = 2,
		int32_t = 4,
		uint32_t = 4,
		sint32_t = 4,
		int64_t = 8,
		uint64_t = 8,
		sint64_t = 8,

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
	enum class BufferType : uint8_t
	{
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
	enum class BufferMemoryProfile : uint8_t
	{
		Automatic,
		CPUOnly,
		DeviceOnly,
		TransferFriendly
	};

	/**
	 * Image type enum.
	 */
	enum class ImageType : uint8_t
	{
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
	enum class ImageUsage : uint8_t
	{
		Graphics = BitShiftLeft(0),
		Storage = BitShiftLeft(1),
		Depth = BitShiftLeft(2),
		Color = BitShiftLeft(3)
	};

	/**
	 * Bitwise OR operator for the image usage.
	 *
	 * @param lhs The left hand side argument.
	 * @param rhs The right hand side argument.
	 * @return The OR-ed value.
	 */
	constexpr ImageUsage operator|(const ImageUsage& lhs, const ImageUsage& rhs) { return static_cast<ImageUsage>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)); }

	/**
	 * Bitwise And operator for the image usage.
	 *
	 * @param lhs The left hand side argument.
	 * @param rhs The right hand side argument.
	 * @return The And-ed value.
	 */
	constexpr ImageUsage operator&(const ImageUsage& lhs, const ImageUsage& rhs) { return static_cast<ImageUsage>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)); }

	/**
	 * Pixel format enum.
	 */
	enum class PixelFormat : uint8_t
	{
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
	enum class AddressMode : uint8_t
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
		MirrorClampToEdge,
	};

	/**
	 * Border color enum.
	 */
	enum class BorderColor : uint8_t
	{
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
	enum class CompareOperator : uint8_t
	{
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
	enum class ImageFilter : uint8_t
	{
		Nearest,
		Linear,
		CubicImage,
	};

	/**
	 *Image mip map mode enum.
	 */
	enum class ImageMipMapMode : uint8_t
	{
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

		float mMipLODBias = 0.0f;

		AddressMode mAddressModeU = AddressMode::Repeat;
		AddressMode mAddressModeV = AddressMode::Repeat;
		AddressMode mAddressModeW = AddressMode::Repeat;

		BorderColor mBorderColor = BorderColor::OpaqueWhiteFLOAT;

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
	enum class ShaderType : uint8_t
	{
		Undefined,
		Vertex, TessellationControl, TessellationEvaluation, Geometry, Fragment,
		Compute,
		RayGen, AnyHit, ClosestHit, RayMiss
	};

	/**
	 * Shader resource type enum.
	 */
	enum class ShaderResourceType : uint8_t
	{
		Sampler,
		CombinedImageSampler,
		SampledImage,
		StorageImage,
		UniformTexelBuffer,
		StorageTexelBuffer,
		UniformBuffer,
		StorageBuffer,
		UniformBufferDynamic,
		StorageBufferDynamic,
		InputAttachment,
		AccelerationStructure,
	};

	/**
	 * Shader resource structure.
	 */
	struct ShaderResource
	{
		ShaderResource() = default;
		ShaderResource(uint32_t setIndex, ShaderResourceType type) : mSetIndex(setIndex), mType(type) {}

		uint32_t mSetIndex = 0;
		ShaderResourceType mType = ShaderResourceType::UniformBuffer;
	};

	/**
	 * Shader attribute structure.
	 */
	struct ShaderAttribute
	{
		ShaderAttribute() = default;
		ShaderAttribute(const std::string& name, uint32_t location, ShaderAttributeDataType type) : mAttributeName(name), mLocation(location), mDataType(type) {}

		const bool operator==(const ShaderAttribute& other) const { return mAttributeName == other.mAttributeName && mLocation == other.mLocation && mDataType == other.mDataType; }

		std::string mAttributeName = "";
		uint32_t mLocation = 0;
		ShaderAttributeDataType mDataType = ShaderAttributeDataType::VEC3;
	};

	/**
	 * Shader resource key structure.
	 */
	struct ShaderResourceKey
	{
		ShaderResourceKey() = default;
		ShaderResourceKey(const uint32_t set, uint32_t binding) : mSetIndex(set), mBindingIndex(binding) {}

		uint32_t mSetIndex = 0;
		uint32_t mBindingIndex = 0;

		constexpr bool operator==(const ShaderResourceKey& other) const { return mSetIndex == other.mSetIndex && mBindingIndex == other.mBindingIndex; }
	};

	using TShaderResourceMap = std::unordered_map<uint32_t, std::unordered_map<uint32_t, ShaderResourceType>>;
	using TShaderAttributeMap = std::unordered_map<uint32_t, std::vector<ShaderAttribute>>;

	/**
	 * Rasterization samples enum.
	 */
	enum class MultiSampleCount : uint8_t
	{
		One = BitShiftLeft(0),
		Two = BitShiftLeft(1),
		Four = BitShiftLeft(2),
		Eight = BitShiftLeft(3),
		Sixteen = BitShiftLeft(4),
		ThirtyTwo = BitShiftLeft(5),
		SixtyFour = BitShiftLeft(6),
	};

	/**
	 * Rasterization samples bitwise OR operator.
	 *
	 * @param lhs The left hand side argument.
	 * @param rhs The right hand side argument.
	 * @return The OR performed samples.
	 */
	constexpr MultiSampleCount operator|(const MultiSampleCount& lhs, const MultiSampleCount& rhs) { return static_cast<MultiSampleCount>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)); }

	/**
	 * Rasterization samples bitwise And operator.
	 *
	 * @param lhs The left hand side argument.
	 * @param rhs The right hand side argument.
	 * @return The And performed samples.
	 */
	constexpr MultiSampleCount operator&(const MultiSampleCount& lhs, const MultiSampleCount& rhs) { return static_cast<MultiSampleCount>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)); }

	/**
	 * Screen bound render target present mode.
	 */
	enum class SwapChainPresentMode : uint8_t
	{
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
		DepthClearValues(float depth, uint32_t stencil) : mDepth(depth), mStencil(stencil) {}

		float mDepth = 0.0f;
		uint32_t mStencil = 0;
	};

	/**
	 * Render target attachment structure.
	 */
	template<class ImageT>
	struct RenderTargetAttachment
	{
		RenderTargetAttachment() = default;
		RenderTargetAttachment(const FColor4D& clearColor) : mClearColor(clearColor) {}
		RenderTargetAttachment(ImageT* pImage, const FColor4D& clearColor) : pImage(std::move(pImage)), mClearColor(clearColor) {}
		RenderTargetAttachment(ImageT* pImage, const DepthClearValues& depthValue) : pImage(std::move(pImage)), mDepthClearValue(depthValue) {}

		ImageT* pImage = nullptr;

		FColor4D mClearColor = FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f);
		DepthClearValues mDepthClearValue = {};
	};

	/**
	 * Query usage enum.
	 * This specify what the query is used for.
	 */
	enum class QueryUsage : uint8_t
	{
		Occlusion,
		PipelineStatistics,
		Timestamp,
		TransformFeedbackStream,
		Performance,
		AccelerationStructureCompactedSize,
		AccelerationStructureSerializationSize
	};

	/**
	 * Flint device bound object.
	 * This object is the base class for all the resources in Flint which are based on a device.
	 */
	template<class DeviceT>
	class DeviceBoundObject : public FObject
	{
		friend DeviceT;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 */
		explicit DeviceBoundObject(DeviceT* pDevice) : pDevice(pDevice)
		{
			if (!pDevice)
				throw std::invalid_argument("Device pointer should not be null!");
		}

		/**
		 * Get the device of this object.
		 *
		 * @return The device pointer.
		 */
		DeviceT* GetDevice() const { return pDevice; }

		/**
		 * Terminate the device bound object.
		 */
		virtual void Terminate() = 0;

	protected:
		DeviceT* pDevice = nullptr;
	};
}

namespace std
{
	template<>
	struct hash<Flint::ShaderAttribute>
	{
		const size_t operator()(const Flint::ShaderAttribute& other) const
		{
			return hash<std::string>()(other.mAttributeName) ^ static_cast<uint8_t>(other.mDataType) ^ other.mLocation;
		}
	};

	template<>
	struct hash<std::vector<Flint::ShaderAttribute>>
	{
		const size_t operator()(const std::vector<Flint::ShaderAttribute>& other) const
		{
			return Flint::Hasher::HashDataBlock(other.data(), sizeof(Flint::ShaderAttribute) * other.size());
		}
	};
}