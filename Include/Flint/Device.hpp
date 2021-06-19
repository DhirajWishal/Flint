// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	class Display;
	class DeviceBoundObject;
	class Instance;
	class CommandBufferList;
	class RenderTarget;
	class ScreenBoundRenderTarget;

	class Buffer;
	class StaggingBuffer;
	class UniformBuffer;
	class StorageBuffer;
	class VertexBuffer;
	class IndexBuffer;

	class Shader;
	class VertexShader;
	class FragmentShader;

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
		Device(Instance& instance, DeviceFlags flags) noexcept : mInstance(instance), mFlags(flags) {}

		/**
		 * Check if the display is compatible with the device.
		 *
		 * @param display: The display to check.
		 * @return Boolean value stating if compatible or not.
		 */
		virtual bool IsDisplayCompatible(const Display& display) = 0;

		/**
		 * Create a new primary command buffer list.
		 * These command buffers are used to submit data to the device.
		 *
		 * @param bufferCount: The number of command buffers in the list.
		 * @return The command buffer list object.
		 */
		virtual CommandBufferList& CreatePrimaryCommandBufferList(UI32 bufferCount) = 0;

		/**
		 * Create a new secondary command buffer list.
		 * Secondary command buffers are used in multi threading and it uses a primary command buffer to submit data to the device.
		 *
		 * @param bufferCount: The number of command buffers in the list.
		 * @param parent: The parent command buffer list to derive information from.
		 * @return The command buffer list object.
		 */
		virtual CommandBufferList& CreateSecondaryCommandBufferList(UI32 bufferCount, CommandBufferList& parent) = 0;

		/**
		 * Terminate a created command buffer list.
		 *
		 * @param commandBufferList: The command buffer list to terminate.
		 */
		virtual void DestroyCommandBufferList(CommandBufferList& commandBufferList) = 0;

		/**
		 * Create a new screen bound render target.
		 * Screen bound render targets render frames to the display. This display must be compatible with the device object.
		 *
		 * @param display: The display object.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The buffer count of the frame buffer.
		 * @return The screen bound render target object.
		 */
		virtual ScreenBoundRenderTarget& CreateScreenBoundRenderTarget(Display& display, const FExtent2D& extent, const UI32 bufferCount) = 0;

		/**
		 * Destroy a created render target.
		 *
		 * @param renderTarget: The render target to destroy.
		 */
		virtual void DestroyRenderTarget(RenderTarget& renderTarget) = 0;

		/**
		 * Create a new stagging buffer.
		 * Stagging buffers are used to transfer data to the device, and between buffers and images.
		 *
		 * @param size: The buffer size.
		 * @return The stagging buffer object.
		 */
		virtual StaggingBuffer& CreateStaggingBuffer(UI64 size) = 0;

		/**
		 * Create a new uniform buffer.
		 * Uniform buffers are used to submit uniform information to the shaders.
		 *
		 * @param size: The buffer size.
		 * @return The uniform buffer object.
		 */
		virtual UniformBuffer& CreateUniformBuffer(UI64 size) = 0;

		/**
		 * Create a new storage buffer.
		 * Storage buffers are used to store information from the shaders.
		 *
		 * @param size: The buffer size.
		 * @return The storage buffer.
		 */
		virtual StorageBuffer& CreateStorageBuffer(UI64 size) = 0;

		/**
		 * Create a new vertex buffer.
		 * Vertex buffers are used to store information about vertexes and submitted to the pipeline to draw geometry.
		 * Switching between vertex buffers are costly so it is recommended to use one large vertex buffer with multiple draw instances.
		 *
		 * @param size: The size of the buffer.
		 * @param descriptor: The vertex descriptor.
		 * @return The vertex buffer object.
		 */
		virtual VertexBuffer& CreateVertexBuffer(UI64 size, const VertexDescriptor& descriptor) = 0;

		/**
		 * Create a new index buffer.
		 * Index buffers are used to store index information and are submitted to the pipeline along with a vertex buffer.
		 * Same as vertex buffers, switching index buffers are costly and is recommended to use one large index buffer along with a vertex buffer to draw.
		 *
		 * @param size: The buffer size.
		 * @param stride: The index stride.
		 * @return The index buffer object.
		 */
		virtual IndexBuffer& CreateIndexBuffer(UI64 size, UI64 stride) = 0;

		/**
		 * Destroy a created buffer.
		 *
		 * @param buffer: The buffer to be destroyed.
		 */
		virtual void DestroyBuffer(Buffer& buffer) = 0;

		/**
		 * Create a new vertex shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param path: The shade code path.
		 * @param type: The shader code type.
		 * @return The vertex shader object.
		 */
		virtual VertexShader& CreateVertexShader(const std::filesystem::path& path, ShaderCodeType type) = 0;

		/**
		 * Create a new vertex shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param code: The shade code.
		 * @param type: The shader code type. Default is SPIR_V.
		 * @return The vertex shader object.
		 */
		virtual VertexShader& CreateVertexShader(const std::vector<UI32>& code, ShaderCodeType type = ShaderCodeType::SPIR_V) = 0;

		/**
		 * Create a new vertex shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param code: The shade code.
		 * @param type: The shader code type. Default is GLSL.
		 * @return The vertex shader object.
		 */
		virtual VertexShader& CreateVertexShader(const std::string& code, ShaderCodeType type = ShaderCodeType::GLSL) = 0;

		/**
		 * Create a new vertex shader.
		 * Fragment shaders are used to color pixels after rasterization in the graphics pipeline.
		 *
		 * @param path: The shade code path.
		 * @param type: The shader code type.
		 * @return The vertex shader object.
		 */
		virtual FragmentShader& CreateFragmentShader(const std::filesystem::path& path, ShaderCodeType type) = 0;

		/**
		 * Create a new vertex shader.
		 * Fragment shaders are used to color pixels after rasterization in the graphics pipeline.
		 *
		 * @param code: The shade code.
		 * @param type: The shader code type. Default is SPIR_V.
		 * @return The vertex shader object.
		 */
		virtual FragmentShader& CreateFragmentShader(const std::vector<UI32>& code, ShaderCodeType type = ShaderCodeType::SPIR_V) = 0;

		/**
		 * Create a new vertex shader.
		 * Fragment shaders are used to color pixels after rasterization in the graphics pipeline.
		 *
		 * @param code: The shade code.
		 * @param type: The shader code type. Default is GLSL.
		 * @return The vertex shader object.
		 */
		virtual FragmentShader& CreateFragmentShader(const std::string& code, ShaderCodeType type = ShaderCodeType::GLSL) = 0;

		/**
		 * Destroy a created shader.
		 *
		 * @param shader: The shader object to destroy.
		 */
		virtual void DestroyShader(Shader& shader) = 0;

	public:
		/**
		 * Wait till the device finish execution.
		 */
		virtual void WaitIdle() = 0;

		/**
		 * Wait for a queue finish its commands.
		 */
		virtual void WaitForQueue() = 0;

	protected:
		/**
		 * Terminate the device object.
		 */
		virtual void Terminate() = 0;

		/**
		 * Terminate a device bound object.
		 *
		 * @param object: The object to terminate.
		 */
		void TerminateDeviceBoundObject(DeviceBoundObject& object) const { object.Terminate(); }

	protected:
		Instance& mInstance;
		DeviceFlags mFlags = DeviceFlags::EXTERNAL | DeviceFlags::GRAPHICS_COMPATIBLE;
	};
}