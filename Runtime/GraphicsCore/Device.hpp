// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Device flags enum.
	 * This determines the device characteristics.
	 */
	enum class DeviceFlags : uint8_t
	{
		// This flag states to use external device (GPU) if available over integrated.
		External = BitShiftLeft(0),

		// This flag states to use only the integrated device if available. If an integrated one doesn't exist but an external one exists, it will automatically select the external device.
		Integrated = BitShiftLeft(1),

		// State that the device is used for graphics. This is a must if graphics needs to be enabled.
		GraphicsCompatible = BitShiftLeft(2),

		// This states the device is used for compute.
		ComputeCompatible = BitShiftLeft(3)
	};

	constexpr DeviceFlags operator|(const DeviceFlags& lhs, const DeviceFlags& rhs) { return DeviceFlags(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)); }
	constexpr DeviceFlags operator&(const DeviceFlags& lhs, const DeviceFlags& rhs) { return DeviceFlags(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)); }

	/**
	 * Flint device object.
	 * This object is the basis for all the resources. Almost all the resources are bound to a device.
	 */
	template<class DerivedT, class InstanceT, class DisplayT, class CommandBufferAllocatorT, class SwapChainT, class ScreenBoundRenderTargetT, class OffScreenRenderTargetT, class BufferT, class ImageT, class ImageSamplerT, class ShaderT, class GraphicsPipelineT, class ComputePipelineT, class GeometryStoreT, class HostSynchronizationPrimitiveT, class DeviceSynchronizationPrimitiveT, class QueryT, class CommandBufferT>
	class Device : public FObject
	{
		friend InstanceT;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pInstance The instance pointer.
		 * @param flags The device flags.
		 */
		Device(InstanceT* pInstance, const DeviceFlags flags) : pInstance(pInstance), m_Flags(flags)
		{
			if (!pInstance)
				throw std::invalid_argument("Instance pointer should not be null!");
		}

		/**
		 * Check if the display is compatible with the device.
		 *
		 * @param pDisplay The display to check.
		 * @return Boolean value stating if compatible or not.
		 */
		virtual bool IsDisplayCompatible(const DisplayT* pDisplay) = 0;

		/**
		 * Create a new command buffer allocator.
		 *
		 * @param bufferCount The number of buffers the allocator should create.
		 * @return The allocator pointer.
		 */
		virtual std::unique_ptr<CommandBufferAllocatorT> CreateCommandBufferAllocator(const uint32_t bufferCount) = 0;

		/**
		 * Create a new secondary command buffer allocator.
		 *
		 * @param bufferCount The number of buffers the allocator should create.
		 * @param pParentAllocator The parent command buffer allocator pointer.
		 * @return The command buffer allocator pointer.
		 */
		virtual std::unique_ptr<CommandBufferAllocatorT> CreateSecondaryCommandBufferAllocator(const uint32_t bufferCount, CommandBufferAllocatorT* pParentAllocator) = 0;

		/**
		 * Create a new swap chain.
		 *
		 * @param pDisplay The display pointer to which the swap chain is attached to.
		 * @param imageCount The number of images in the swap chain.
		 * @param presentMode The swap chain present mode.
		 * @return The swap chain pointer.
		 */
		virtual std::unique_ptr<SwapChainT> CreateSwapChain(DisplayT* pDisplay, uint32_t imageCount, const SwapChainPresentMode presentMode) = 0;

		/**
		 * Create a new screen bound render target.
		 * Screen bound render targets render frames to the display. This display must be compatible with the device object.
		 *
		 * @param pDisplay The display object.
		 * @param extent The extent of the render target.
		 * @param bufferCount The buffer count of the frame buffer.
		 * @param imageAttachments The image attachments to use.
		 * @param presentMode The swap chain present mode.
		 * @param swapChainClearColor The swap chain's clear color.
		 * @return The screen bound render target object.
		 */
		virtual std::unique_ptr<ScreenBoundRenderTargetT> CreateScreenBoundRenderTarget(DisplayT* pDisplay, const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachment<ImageT>>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor = FColor4D(0.0f)) = 0;

		/**
		 * Create a new off screen render target.
		 * Off screen render targets are used to perform calculations or other off screen operations required.
		 *
		 * @param extent The render target extent.
		 * @param bufferCount The number of buffers to use.
		 * @param imageAttachments The image attachments which the render target uses.
		 * @return The render target pointer.
		 */
		virtual std::unique_ptr<OffScreenRenderTargetT> CreateOffScreenRenderTarget(const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachment<ImageT>>& imageAttachments) = 0;

		/**
		 * Create a new buffer.
		 *
		 * @param type The buffer type.
		 * @param size The buffer size.
		 * @param profile The memory profile of the buffer. Default is BufferMemoryProfile::Automatic.
		 * @return The buffer object.
		 */
		virtual std::unique_ptr<BufferT> CreateBuffer(const BufferType type, const uint64_t size, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic) = 0;

		/**
		 * Create a new image.
		 *
		 * @param type The image type.
		 * @param usage The image usage.
		 * @param extent The image extent.
		 * @param format The image format.
		 * @param layers The number of layers in the image.
		 * @param mipLevels The mip levels of the image.
		 * @param pImageData The image data pointer to load data from.
		 * @param sampleCount The image multi sample count.
		 * @return The newly created image.
		 */
		virtual std::unique_ptr<ImageT> CreateImage(const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const uint8_t layers, const uint32_t mipLevels, const void* pImageData, const MultiSampleCount sampleCount = MultiSampleCount::One) = 0;

		/**
		 * Create a new image sampler.
		 *
		 * @param specification The sampler specification.
		 * @return The newly created sampler.
		 */
		virtual std::unique_ptr<ImageSamplerT> CreateImageSampler(const ImageSamplerSpecification& specification) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type The type of the shader.
		 * @param path The shade code path.
		 * @return The vertex shader object.
		 */
		virtual std::unique_ptr<ShaderT> CreateShader(const ShaderType type, const std::filesystem::path& path) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type The type of the shader.
		 * @param code The shade code.
		 * @return The vertex shader object.
		 */
		virtual std::unique_ptr<ShaderT> CreateShader(const ShaderType type, const std::vector<uint32_t>& code) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type The type of the shader.
		 * @param code The shade code.
		 * @return The vertex shader object.
		 */
		virtual std::unique_ptr<ShaderT> CreateShader(const ShaderType type, const std::string& code) = 0;

		/**
		 * Create a new graphics pipeline bound to a screen bound render target.
		 *
		 * @param pipelineName The unique name of the pipeline.
		 * @param pScreenBoundRenderTarget The screen bound render target pointer.
		 * @param pVertexShader The vertex shader pointer.
		 * @param pTessellationControlShader The tessellation control shader pointer.
		 * @param pTessellationEvaluationShader The tessellation evaluation shader pointer.
		 * @param pGeometryShader The geometry shader pointer.
		 * @param pFragmentShader The fragment shader pointer.
		 * @param specification The pipeline specification.
		 * @return The pipeline pointer.
		 */
		virtual std::unique_ptr<GraphicsPipelineT> CreateGraphicsPipeline(const std::string& pipelineName, ScreenBoundRenderTargetT* pScreenBoundRenderTarget, std::unique_ptr<ShaderT>&& pVertexShader, std::unique_ptr<ShaderT>&& pTessellationControlShader, std::unique_ptr<ShaderT>&& pTessellationEvaluationShader, std::unique_ptr<ShaderT>&& pGeometryShader, std::unique_ptr<ShaderT>&& pFragmentShader, const GraphicsPipelineSpecification& specification) = 0;

		/**
		 * Create a new graphics pipeline bound to a off screen render target.
		 *
		 * @param pipelineName The unique name of the pipeline.
		 * @param pOffScreenRenderTarget The off screen render target pointer.
		 * @param pVertexShader The vertex shader pointer.
		 * @param pTessellationControlShader The tessellation control shader pointer.
		 * @param pTessellationEvaluationShader The tessellation evaluation shader pointer.
		 * @param pGeometryShader The geometry shader pointer.
		 * @param pFragmentShader The fragment shader pointer.
		 * @param specification The pipeline specification.
		 * @return The pipeline pointer.
		 */
		virtual std::unique_ptr<GraphicsPipelineT> CreateGraphicsPipeline(const std::string& pipelineName, OffScreenRenderTargetT* pOffScreenRenderTarget, std::unique_ptr<ShaderT>&& pVertexShader, std::unique_ptr<ShaderT>&& pTessellationControlShader, std::unique_ptr<ShaderT>&& pTessellationEvaluationShader, std::unique_ptr<ShaderT>&& pGeometryShader, std::unique_ptr<ShaderT>&& pFragmentShader, const GraphicsPipelineSpecification& specification) = 0;

		/**
		 * Create a new compute pipeline.
		 *
		 * @param pipelineName The unique name of the pipeline.
		 * @param pShader The compute shader pointer.
		 * @return The pipeline pointer.
		 */
		virtual std::unique_ptr<ComputePipelineT> CreateComputePipeline(const std::string& pipelineName, std::unique_ptr<ShaderT>&& pShader) = 0;

		/**
		 * Create a new geometry store.
		 *
		 * @param vertexAttributes The vertex attributes of the store.
		 * @param indexSize The size of a single index.
		 * @param profile The memory profile of the geometry store. Default is BufferMemoryProfile::Automatic.
		 * @return The newly created geometry store pointer.
		 */
		virtual std::shared_ptr<GeometryStoreT> CreateGeometryStore(const std::vector<ShaderAttribute>& vertexAttributes, uint64_t indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic) = 0;

		/**
		 * Create a new host synchronization primitive.
		 * Host synchronization primitives are used to synchronize the client/ host application.
		 *
		 * @return The primitive pointer.
		 */
		virtual std::unique_ptr<HostSynchronizationPrimitiveT> CreateHostSynchronizationPrimitive() = 0;

		/**
		 * Create a new device synchronization primitive.
		 * Device synchronization primitives are used to synchronize the device.
		 *
		 * @return The primitive pointer.
		 */
		virtual std::unique_ptr<DeviceSynchronizationPrimitiveT> CreateDeviceSynchronizationPrimitive() = 0;

		/**
		 * Create a new query object.
		 *
		 * @param usage The query usage.
		 * @param queryCount The number of query primitives.
		 * @return The query pointer.
		 */
		virtual std::unique_ptr<QueryT> CreateQuery(const QueryUsage usage, const uint32_t queryCount) = 0;

	public:
		/**
		 * Submit a graphics command buffer to the device.
		 *
		 * @param pCommandBuffer The command buffer pointer.
		 * @param pPrimitive The synchronization primitive to be flagged upon completion. Default is nullptr.
		 */
		virtual void SubmitGraphicsCommandBuffer(const CommandBufferT* pCommandBuffer, HostSynchronizationPrimitiveT* pPrimitive = nullptr) = 0;

		/**
		 * Submit graphics command buffers to the device.
		 *
		 * @param pCommandBuffers The command buffer pointers.
		 * @param pPrimitive The synchronization primitive to be flagged upon completion. Default is nullptr.
		 */
		virtual void SubmitGraphicsCommandBuffers(const std::vector<CommandBufferT*>& pCommandBuffers, HostSynchronizationPrimitiveT* pPrimitive = nullptr) = 0;

		/**
		 * Submit a compute command buffer to the device.
		 *
		 * @param pCommandBuffer The command buffer pointer.
		 * @param pPrimitive The synchronization primitive to be flagged upon completion. Default is nullptr.
		 */
		virtual void SubmitComputeCommandBuffer(const CommandBufferT* pCommandBuffer, HostSynchronizationPrimitiveT* pPrimitive = nullptr) = 0;

		/**
		 * Submit compute command buffers to the device.
		 *
		 * @param pCommandBuffers The command buffer pointers.
		 * @param pPrimitive The synchronization primitive to be flagged upon completion. Default is nullptr.
		 */
		virtual void SubmitComputeCommandBuffers(const std::vector<CommandBufferT*>& pCommandBuffers, HostSynchronizationPrimitiveT* pPrimitive = nullptr) = 0;

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
		const bool IsGraphicsCompatible() const { return (m_Flags & DeviceFlags::GraphicsCompatible) == DeviceFlags::GraphicsCompatible; }

		/**
		 * Check if the device is compute compatible.
		 *
		 * @return The compatibility status (boolean).
		 */
		const bool IsComputeCompatible() const { return (m_Flags & DeviceFlags::ComputeCompatible) == DeviceFlags::ComputeCompatible; }

	public:
		/**
		 * Get the current instance.
		 *
		 * @return The instance pointer.
		 */
		InstanceT* GetInstance() const { return pInstance; }

		/**
		 * Get the supported rasterization samples.
		 *
		 * @return The samples.
		 */
		virtual MultiSampleCount GetSupportedMultiSampleCount() const = 0;

	protected:
		InstanceT* pInstance = nullptr;
		DeviceFlags m_Flags = DeviceFlags::External | DeviceFlags::GraphicsCompatible;
	};
}