// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

#include <unordered_map>

namespace Flint
{
	class Display;
	class DeviceBoundObject;
	class Instance;

	class CommandBufferAllocator;
	class CommandBuffer;

	class RenderTarget;
	class ScreenBoundRenderTarget;
	class OffScreenRenderTarget;

	class Buffer;
	class Image;
	class ImageSampler;
	class Shader;

	class Pipeline;
	class GraphicsPipeline;
	class ComputePipeline;

	struct GraphicsPipelineSpecification;
	struct ShaderAttribute;

	class GeometryStore;

	/**
	 * Device flags enum.
	 * This determines the device characteristics.
	 */
	enum class DeviceFlags : UI8 {
		/**
		 * This flag states to use external device (GPU) if available over integrated.
		 */
		EXTERNAL = BIT_SHIFT(0),

		/**
		 * This flag states to use only the integrated device if available. If an integrated one doesn't exist but an external one exists, it will automatically select the external device.
		 */
		 INTEGRATED = BIT_SHIFT(1),

		 /**
		  * State that the device is used for graphics. This is a must if graphics needs to be enabled.
		  */
		  GRAPHICS_COMPATIBLE = BIT_SHIFT(2),

		  /**
		   * This states the device is used for compute.
		   */
		   COMPUTE_COMPATIBLE = BIT_SHIFT(3)
	};

	constexpr DeviceFlags operator|(const DeviceFlags& lhs, const DeviceFlags& rhs) { return DeviceFlags(static_cast<UI8>(lhs) | static_cast<UI8>(rhs)); }
	constexpr DeviceFlags operator&(const DeviceFlags& lhs, const DeviceFlags& rhs) { return DeviceFlags(static_cast<UI8>(lhs) & static_cast<UI8>(rhs)); }

	/**
	 * Flint device object.
	 * This object is the basis for all the resources. Almost all the resources are bound to a device.
	 */
	class Device : public FObject
	{
		friend Instance;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pInstance: The instance pointer.
		 * @param flags: The device flags.
		 */
		Device(const std::shared_ptr<Instance>& pInstance, const DeviceFlags flags);

		/**
		 * Check if the display is compatible with the device.
		 *
		 * @param pDisplay: The display to check.
		 * @return Boolean value stating if compatible or not.
		 */
		virtual bool IsDisplayCompatible(const std::shared_ptr<Display>& pDisplay) = 0;

		/**
		 * Create a new command buffer allocator.
		 *
		 * @param bufferCount: The number of buffers the allocator should create.
		 * @return The allocator pointer.
		 */
		virtual std::shared_ptr<CommandBufferAllocator> CreateCommandBufferAllocator(const UI32 bufferCount) = 0;

		/**
		 * Create a new secondary command buffer allocator.
		 *
		 * @param bufferCount: The number of buffers the allocator should create.
		 * @param pParentAllocator: The parent command buffer allocator pointer.
		 * @return The command buffer allocator pointer.
		 */
		virtual std::shared_ptr<CommandBufferAllocator> CreateSecondaryCommandBufferAllocator(const UI32 bufferCount, const std::shared_ptr<CommandBufferAllocator>& pParentAllocator) = 0;

		/**
		 * Create a new screen bound render target.
		 * Screen bound render targets render frames to the display. This display must be compatible with the device object.
		 *
		 * @param display: The display object.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The buffer count of the frame buffer.
		 * @param imageAttachments: The image attachments to use.
		 * @param presentMode: The swap chain present mode.
		 * @return The screen bound render target object.
		 */
		virtual std::shared_ptr<ScreenBoundRenderTarget> CreateScreenBoundRenderTarget(const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode) = 0;

		/**
		 * Create a new off screen render target.
		 * Off screen render targets are used to perform calculations or other off screen operations required.
		 *
		 * @param extent: The render target extent.
		 * @param bufferCount: The number of buffers to use.
		 * @param imageAttachments: The image attachments which the render target uses.
		 * @return The render target pointer.
		 */
		virtual std::shared_ptr<OffScreenRenderTarget> CreateOffScreenRenderTarget(const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments) = 0;

		/**
		 * Create a new buffer.
		 *
		 * @param type: The buffer type.
		 * @param size: The buffer size.
		 * @param profile: The memory profile of the buffer. Default is BufferMemoryProfile::AUTOMATIC.
		 * @return The buffer object.
		 */
		virtual std::shared_ptr<Buffer> CreateBuffer(const BufferType type, const UI64 size, const BufferMemoryProfile profile = BufferMemoryProfile::AUTOMATIC) = 0;

		/**
		 * Create a new image.
		 *
		 * @param type: The image type.
		 * @param usage: The image usage.
		 * @param extent: The image extent.
		 * @param format: The image format.
		 * @param layers: The number of layers in the image.
		 * @param mipLevels: The mip levels of the image.
		 * @param pImageData: The image data pointer to load data from.
		 * @param sampleCount: The image multi sample count.
		 * @return The newly created image.
		 */
		virtual std::shared_ptr<Image> CreateImage(const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const UI8 layers, const UI32 mipLevels, const void* pImageData, const MultiSampleCount sampleCount = MultiSampleCount::BITS_1) = 0;

		/**
		 * Create a new image sampler.
		 *
		 * @param specification: The sampler specification.
		 * @return The newly created sampler.
		 */
		virtual std::shared_ptr<ImageSampler> CreateImageSampler(const ImageSamplerSpecification& specification) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type: The type of the shader.
		 * @param path: The shade code path.
		 * @return The vertex shader object.
		 */
		virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::filesystem::path& path) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type: The type of the shader.
		 * @param code: The shade code.
		 * @return The vertex shader object.
		 */
		virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::vector<UI32>& code) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type: The type of the shader.
		 * @param code: The shade code.
		 * @return The vertex shader object.
		 */
		virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::string& code) = 0;

		/**
		 * Create a new graphics pipeline bound to a screen bound render target.
		 *
		 * @param pipelineName: The unique name of the pipeline.
		 * @param pScreenBoundRenderTarget: The screen bound render target pointer.
		 * @param pVertexShader: The vertex shader pointer.
		 * @param pTessellationControlShader: The tessellation control shader pointer.
		 * @param pTessellationEvaluationShader: The tessellation evaluation shader pointer.
		 * @param pGeometryShader: The geometry shader pointer.
		 * @param pFragmentShader: The fragment shader pointer.
		 * @param specification: The pipeline specification.
		 * @return The pipeline pointer.
		 */
		virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(
			const std::string& pipelineName,
			const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
			const std::shared_ptr<Shader>& pVertexShader,
			const std::shared_ptr<Shader>& pTessellationControlShader,
			const std::shared_ptr<Shader>& pTessellationEvaluationShader,
			const std::shared_ptr<Shader>& pGeometryShader,
			const std::shared_ptr<Shader>& pFragmentShader,
			const GraphicsPipelineSpecification& specification) = 0;

		/**
		 * Create a new graphics pipeline bound to a off screen render target.
		 *
		 * @param pipelineName: The unique name of the pipeline.
		 * @param pOffScreenRenderTarget: The off screen render target pointer.
		 * @param pVertexShader: The vertex shader pointer.
		 * @param pTessellationControlShader: The tessellation control shader pointer.
		 * @param pTessellationEvaluationShader: The tessellation evaluation shader pointer.
		 * @param pGeometryShader: The geometry shader pointer.
		 * @param pFragmentShader: The fragment shader pointer.
		 * @param specification: The pipeline specification.
		 * @return The pipeline pointer.
		 */
		virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(
			const std::string& pipelineName,
			const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget,
			const std::shared_ptr<Shader>& pVertexShader,
			const std::shared_ptr<Shader>& pTessellationControlShader,
			const std::shared_ptr<Shader>& pTessellationEvaluationShader,
			const std::shared_ptr<Shader>& pGeometryShader,
			const std::shared_ptr<Shader>& pFragmentShader,
			const GraphicsPipelineSpecification& specification) = 0;

		/**
		 * Create a new compute pipeline.
		 *
		 * @param pipelineName: The unique name of the pipeline.
		 * @param pShader: The compute shader pointer.
		 * @return The pipeline pointer.
		 */
		virtual std::shared_ptr<ComputePipeline> CreateComputePipeline(const std::string& pipelineName, const std::shared_ptr<Shader>& pShader) = 0;

		/**
		 * Create a new geometry store.
		 *
		 * @param vertexAttributes: The vertex attributes of the store.
		 * @param indexSize: The size of a single index.
		 * @param profile: The memory profile of the geometry store. Default is BufferMemoryProfile::AUTOMATIC.
		 * @return The newly created geometry store pointer.
		 */
		virtual std::shared_ptr<GeometryStore> CreateGeometryStore(const std::unordered_map<UI32, std::vector<ShaderAttribute>>& vertexAttributes, UI64 indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::AUTOMATIC) = 0;

	public:
		/**
		 * Submit graphics command buffers to the device.
		 *
		 * @param pCommandBuffers: The command buffer pointers.
		 */
		virtual void SubmitGraphicsCommandBuffers(const std::vector<std::shared_ptr<CommandBuffer>>& pCommandBuffers) = 0;

		/**
		 * Submit compute command buffers to the device.
		 *
		 * @param pCommandBuffers: The command buffer pointers.
		 */
		virtual void SubmitComputeCommandBuffers(const std::vector<std::shared_ptr<CommandBuffer>>& pCommandBuffers) = 0;

		/**
		 * Wait till the device finish execution.
		 */
		virtual void WaitIdle() = 0;

		/**
		 * Wait for a queue finish its commands.
		 */
		virtual void WaitForQueue() = 0;

	public:
		/**
		 * Terminate the device object.
		 */
		virtual void Terminate() = 0;

	public:
		/**
		 * Check if the device is graphics compatible.
		 *
		 * @return The compatibility status (boolean).
		 */
		const bool IsGraphicsCompatible() const { return (mFlags & DeviceFlags::GRAPHICS_COMPATIBLE) == DeviceFlags::GRAPHICS_COMPATIBLE; }

		/**
		 * Check if the device is compute compatible.
		 *
		 * @return The compatibility status (boolean).
		 */
		const bool IsComputeCompatible() const { return (mFlags & DeviceFlags::COMPUTE_COMPATIBLE) == DeviceFlags::COMPUTE_COMPATIBLE; }

	public:
		/**
		 * Get the supported rasterization samples.
		 *
		 * @return The samples.
		 */
		virtual MultiSampleCount GetSupportedMultiSampleCount() const = 0;

	protected:
		std::shared_ptr<Instance> pInstance = nullptr;
		DeviceFlags mFlags = DeviceFlags::EXTERNAL | DeviceFlags::GRAPHICS_COMPATIBLE;
	};
}