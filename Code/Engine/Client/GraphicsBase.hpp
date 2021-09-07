// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"
#include "GraphicsCore/Display.hpp"

#include "Engine/ShaderCompiler.hpp"

namespace Flint
{
	/**
	 * Flint graphics base class.
	 * This is the base class for the flint graphics engine and contains all the basic information required.
	 */
	class GraphicsBase
	{
	public:
		GraphicsBase() = default;

		/**
		 * Create a new instance.
		 *
		 * @param enableValidation: Whether or not to enable backend validation. We do not recommend using this setting for the final release but is recommended for debugging.
		 */
		void CreateInstance(bool enablevalidation);

		/**
		 * Destroy the created instance.
		 */
		void DestroyInstance();

		/**
		 * Set an instance pointer.
		 *
		 * @param pInstance: The instance to set.
		 */
		void SetInstance(const std::shared_ptr<Instance>& pInstance);

		/**
		 * Create a new device.
		 * The first device will be automatically selected as the default device.
		 *
		 * @param flags: The device flags.
		 */
		void CreateNewDevice(const DeviceFlags flags);

		/**
		 * Set devices.
		 *
		 * @param pDevices: The devices to set.
		 */
		void SetDevices(const std::vector<std::shared_ptr<Device>>& pDevices);

		/**
		 * Select a new default device.
		 *
		 * @param index: The index of the device to set.
		 */
		void SetDefaultDevice(const UI32 index);

		/**
		 * Get the default device pointer.
		 * 
		 * @return The device pointer.
		 */
		const std::shared_ptr<Device> GetDefaultDevice() const;

		/**
		 * Destroy a device from the device list.
		 *
		 * @param index: The index of the device to destroy.
		 */
		void DestroyDevice(const UI32 index);

		/**
		 * Destroy all the devices.
		 */
		void DestroyAllDevices();

	public:
		/**
		 * Create the default screen bound render target.
		 * All the models are rendered to the default render target if not explicitly specified.
		 * The name of the render target is "Default".
		 *
		 * @param extent: The extent of the render target. Default is std::numeric_limits<UI32>::max().
		 * @param windowTitle: The title of the window. Default is "Flint".
		 * @return The created render target pointer.
		 */
		std::shared_ptr<ScreenBoundRenderTarget> CreateDefaultScreenBoundRenderTarget(const FBox2D& extent = { std::numeric_limits<UI32>::max() }, const std::string& windowTitle = "Flint");

		/**
		 * Set the default render target.
		 *
		 * @param pRenderTarget: The render target pointer to set.
		 */
		void SetDefaultScreenBoundRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget);

		/**
		 * Get the screen bound render target.
		 * 
		 * @param name: The name of the render target.
		 * @return The render target pointer.
		 */
		const std::shared_ptr<ScreenBoundRenderTarget> GetScreenBoundRenderTarget(const std::string& name) const;

		/**
		 * Get the default screen bound render target.
		 * 
		 * @return The render target pointer.
		 */
		const std::shared_ptr<ScreenBoundRenderTarget> GetDefaultScreenBoundRenderTarget() const;

	public:
		/**
		 * Create a shader from binary file.
		 *
		 * @param asset: The shader binary asset file.
		 * @param type: The shader type.
		 * @return The shader pointer.
		 */
		std::shared_ptr<Shader> CreateShaderFromBinary(const std::filesystem::path& asset, const ShaderType type);

		/**
		 * Compile and create the shader.
		 *
		 * @param asset: The shader code file.
		 * @param codeType: The shader code type.
		 * @param type: The shader type.
		 */
		std::shared_ptr<Shader> CompileAndCreateShader(const std::filesystem::path& asset, const ShaderCodeType codeType, const ShaderType type);

	public:
		/**
		 * Get a created graphics pipeline using its name.
		 *
		 * @param name: The name of the pipeline.
		 * @return The pipeline pointer.
		 */
		std::shared_ptr<GraphicsPipeline> GetGraphicsPipeline(const std::string& name) const;

		/**
		 * Create a new graphics pipeline.
		 *
		 * @param name: The name of the pipeline.
		 * @param pScreenBoundRenderTarget: The screen bound render target to which the pipeline is bound to.
		 * @param pVertexShader: The vertex shader pointer.
		 * @param pFragmentShader: The fragment shader pointer.
		 * @param specification: The pipeline specification.
		 * @retrun The pipeline pointer.
		 */
		std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(const std::string& name, const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pFragmentShader, const GraphicsPipelineSpecification& specification);

	protected:
		std::shared_ptr<Instance> pInstance = nullptr;
		std::shared_ptr<Device> pDefaultDevice = nullptr;

		std::vector<std::shared_ptr<Device>> pDevices = {};

		std::unordered_map<std::string, std::shared_ptr<GraphicsPipeline>> pGraphicsPipelines = {};
		std::unordered_map<std::string, std::shared_ptr<ScreenBoundRenderTarget>> pScreenBoundRenderTargets = {};
	};
}