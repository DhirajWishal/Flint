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

	class Buffer;
	class Shader;

	class Pipeline;
	class GraphicsPipeline;

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
		 * Terminate a created command buffer list.
		 *
		 * @param pCommandBufferList: The command buffer list to terminate.
		 */
		virtual void DestroyCommandBufferList(const std::shared_ptr<CommandBufferList>& pCommandBufferList) = 0;

		/**
		 * Create a new screen bound render target.
		 * Screen bound render targets render frames to the display. This display must be compatible with the device object.
		 *
		 * @param display: The display object.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The buffer count of the frame buffer.
		 * @return The screen bound render target object.
		 */
		virtual std::shared_ptr<ScreenBoundRenderTarget> CreateScreenBoundRenderTarget(const std::shared_ptr<Display>& pDisplay, const FExtent2D& extent, const UI32 bufferCount) = 0;

		/**
		 * Destroy a created render target.
		 *
		 * @param pRenderTarget: The render target to destroy.
		 */
		virtual void DestroyRenderTarget(const std::shared_ptr<RenderTarget>& pRenderTarget) = 0;

		/**
		 * Create a new buffer.
		 *
		 * @param type: The buffer type.
		 * @param size: The buffer size.
		 * @return The buffer object.
		 */
		virtual std::shared_ptr<Buffer> CreateBuffer(BufferType type, UI64 size) = 0;

		/**
		 * Destroy a created buffer.
		 *
		 * @param pBuffer: The buffer to be destroyed.
		 */
		virtual void DestroyBuffer(const std::shared_ptr<Buffer>& pBuffer) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type: The type of the shader.
		 * @param path: The shade code path.
		 * @param codeType: The shader code type.
		 * @return The vertex shader object.
		 */
		virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::filesystem::path& path, ShaderCodeType codeType) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type: The type of the shader.
		 * @param code: The shade code.
		 * @param codeType: The shader code type. Default is SPIR_V.
		 * @return The vertex shader object.
		 */
		virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::vector<UI32>& code, ShaderCodeType codeType = ShaderCodeType::SPIR_V) = 0;

		/**
		 * Create a new shader.
		 * Vertex shaders are used to perform computations per vertex in the graphics pipeline.
		 *
		 * @param type: The type of the shader.
		 * @param code: The shade code.
		 * @param codeType: The shader code type. Default is GLSL.
		 * @return The vertex shader object.
		 */
		virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::string& code, ShaderCodeType codeType = ShaderCodeType::GLSL) = 0;

		/**
		 * Destroy a created shader.
		 *
		 * @param pShader: The shader object to destroy.
		 */
		virtual void DestroyShader(const std::shared_ptr<Shader>& pShader) = 0;

		/**
		 * Create a new graphics pipeline bound to this render target.
		 *
		 * @param pipelineName: The unique name of the pipeline.
		 * @param pRenderTarget: The screen bound render target pointer.
		 * @param pVertexShader: The vertex shader pointer.
		 * @param pTessellationControlShader: The tessellation control shader pointer.
		 * @param pTessellationEvaluationShader: The tessellation evaluation shader pointer.
		 * @param pGeometryShader: The geometry shader pointer.
		 * @param pFragmentShader: The fragment shader pointer.
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
		 * Destroy the created pipeline.
		 *
		 * @param pPipeline: The pipeline pointer.
		 */
		virtual void DestroyPipeline(const std::shared_ptr<Pipeline>& pPipeline) = 0;

		/**
		 * Create a new geometry store.
		 *
		 * @param vertexAttributes: The vertex attributes of the store.
		 * @param indexSize: The size of a single index.
		 * @return The newly created geometry store pointer.
		 */
		virtual std::shared_ptr<GeometryStore> CreateGeometryStore(const std::unordered_map<UI32, std::vector<ShaderAttribute>>& vertexAttributes, UI64 indexSize) = 0;

		/**
		 * Destroy a created geometry store.
		 *
		 * @param pGeometryStore: The geometry store pointer.
		 */
		virtual void DestroyGeometryStore(const std::shared_ptr<GeometryStore>& pGeometryStore) = 0;

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
		std::shared_ptr<Instance> pInstance = nullptr;
		DeviceFlags mFlags = DeviceFlags::EXTERNAL | DeviceFlags::GRAPHICS_COMPATIBLE;
	};
}