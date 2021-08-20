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
	class CommandBufferList;
	class RenderTarget;
	class ScreenBoundRenderTarget;
	class OffScreenRenderTarget;
	class OffScreenRenderTargetFactory;

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
		Device(const std::shared_ptr<Instance>& pInstance, DeviceFlags flags);

		/**
		 * Check if the display is compatible with the device.
		 *
		 * @param pDisplay: The display to check.
		 * @return Boolean value stating if compatible or not.
		 */
		virtual bool IsDisplayCompatible(const std::shared_ptr<Display>& pDisplay) = 0;

		/**
		 * Create a new primary command buffer list.
		 * These command buffers are used to submit data to the device.
		 *
		 * @param bufferCount: The number of command buffers in the list.
		 * @return The command buffer list object.
		 */
		virtual std::shared_ptr<CommandBufferList> CreatePrimaryCommandBufferList(UI32 bufferCount) = 0;

		/**
		 * Create a new secondary command buffer list.
		 * Secondary command buffers are used in multi threading and it uses a primary command buffer to submit data to the device.
		 *
		 * @param bufferCount: The number of command buffers in the list.
		 * @param pParent: The parent command buffer list to derive information from.
		 * @return The command buffer list object.
		 */
		virtual std::shared_ptr<CommandBufferList> CreateSecondaryCommandBufferList(UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pParent) = 0;

		/**
		 * Create a new screen bound render target.
		 * Screen bound render targets render frames to the display. This display must be compatible with the device object.
		 *
		 * @param display: The display object.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The buffer count of the frame buffer.
		 * @param threadCount: The number of worker threads used. Default is 0.
		 * @return The screen bound render target object.
		 */
		virtual std::shared_ptr<ScreenBoundRenderTarget> CreateScreenBoundRenderTarget(const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, UI32 threadCount = 0) = 0;

		/**
		 * Create a new off screen render target factory.
		 * These factories are used to create different types of render targets, like Shadow maps.
		 * 
		 * @return The factory pointer.
		 */
		virtual std::shared_ptr<OffScreenRenderTargetFactory> CreateOffScreenRenderTargetFactory() = 0;

		/**
		 * Create a new buffer.
		 *
		 * @param type: The buffer type.
		 * @param size: The buffer size.
		 * @param profile: The memory profile of the buffer. Default is BufferMemoryProfile::AUTOMATIC.
		 * @return The buffer object.
		 */
		virtual std::shared_ptr<Buffer> CreateBuffer(BufferType type, UI64 size, BufferMemoryProfile profile = BufferMemoryProfile::AUTOMATIC) = 0;

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
		 * @return The newly created image.
		 */
		virtual std::shared_ptr<Image> CreateImage(const ImageType type, ImageUsage usage, const FBox3D& extent, PixelFormat format, UI8 layers, UI32 mipLevels, const void* pImageData) = 0;

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
		virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::filesystem::path& path) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type: The type of the shader.
		 * @param code: The shade code.
		 * @return The vertex shader object.
		 */
		virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::vector<UI32>& code) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type: The type of the shader.
		 * @param code: The shade code.
		 * @return The vertex shader object.
		 */
		virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::string& code) = 0;

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
		virtual std::shared_ptr<GeometryStore> CreateGeometryStore(const std::unordered_map<UI32, std::vector<ShaderAttribute>>& vertexAttributes, UI64 indexSize, BufferMemoryProfile profile = BufferMemoryProfile::AUTOMATIC) = 0;

	public:
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
		 * Get the supported rasterization samples.
		 * 
		 * @return The samples.
		 */
		virtual RasterizationSamples GetSupportedRasterizationSamples() const = 0;

	protected:
		/**
		 * Terminate the device object.
		 */
		virtual void Terminate() = 0;

	protected:
		std::shared_ptr<Instance> pInstance = nullptr;
		DeviceFlags mFlags = DeviceFlags::EXTERNAL | DeviceFlags::GRAPHICS_COMPATIBLE;
	};
}