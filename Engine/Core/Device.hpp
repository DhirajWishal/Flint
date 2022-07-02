// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "Types.hpp"
#include "ShaderCode.hpp"

#include "Camera/Camera.hpp"

namespace Flint
{
	class Buffer;
	class Rasterizer;
	class RayTracer;
	class Window;
	class RasterizingProgram;
	class StaticModel;
	class Texture2D;

	/**
	 * Device class.
	 * This class contains everything that's needed for a single device instance.
	 *
	 * Usually an application requires just one instance of this class. But the user isn't restricted to do that in any way. But make sure that the devices are destroyed before
	 * destroying the instance which is used to create this.
	 */
	class Device : public FObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pInstance The instance to create the device with.
		 */
		explicit Device(const std::shared_ptr<Instance>& pInstance);

		/**
		 * Default virtual destructor.
		 */
		virtual ~Device() = default;

		/**
		 * Halt the device till all the commands and everything else is executed.
		 */
		virtual void waitIdle() = 0;

		/**
		 * Create a new buffer.
		 *
		 * @param size The size of the buffer.
		 * @param usage The buffer usage.
		 * @param pDataStore The data store pointer to copy everything from. Make sure that the raw buffer's size is the same or more than the buffer's size. Default is nullptr.
		 * @return The buffer pointer.
		 */
		[[nodiscrad]] virtual std::shared_ptr<Buffer> createBuffer(uint64_t size, BufferUsage usage, const std::byte* pDataStore = nullptr) = 0;

		/**
		 * Create a new rasterizer.
		 *
		 * @param camera The camera from which all the models are drawn from.
		 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
		 * @param attachmentDescriptions The attachment descriptions.
		 * @param multisample The multisample count. Default is One.
		 * @param exclusiveBuffering Whether or not to use one buffer/ attachment per frame. Default is false.
		 * @return The rasterizer pointer.
		 */
		[[nodiscard]] virtual std::shared_ptr<Rasterizer> createRasterizer(Camera& camera, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One, bool exclusiveBuffering = false) = 0;

		/**
		 * Create a new ray tracer.
		 *
		 * @param camera The camera reference which is used to generate the images using.
		 * @param frameCount The number of frames to use.
		 * @return The ray tracer pointer.
		 */
		[[nodiscard]] virtual std::shared_ptr<RayTracer> createRayTracer(Camera& camera, uint32_t frameCount) = 0;

		/**
		 * Create a new window.
		 *
		 * @param title The window title.
		 * @param width The width of the window. If set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
		 * @param height The height of the window. Same as the width, if set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
		 * @return The window pointer.
		 */
		[[nodiscard]] virtual std::shared_ptr<Window> createWindow(std::string&& title, uint32_t width = -1, uint32_t height = -1) = 0;

		/**
		 * Create a new rasterizing program.
		 *
		 * @param vertexShader The vertex shader code.
		 * @param fragmentShader The fragment shader code.
		 * @return The rasterizing program pointer.
		 */
		[[nodiscard]] virtual std::shared_ptr<RasterizingProgram> createRasterizingProgram(ShaderCode&& vertexShader, ShaderCode&& fragementShader) = 0;

		/**
		 * Create a new static model.
		 *
		 * @param assetFile The asset file to load the data from.
		 * @return The loaded static model.
		 */
		[[nodiscard]] virtual std::shared_ptr<StaticModel> createStaticModel(std::filesystem::path&& assetFile) = 0;

		/**
		 * Create a new 2D texture image.
		 *
		 * @param width The width of the image.
		 * @param height The height of the image.
		 * @param mipLevels The texture's mip levels. If this is set to 0 (default), the best value is set.
		 * @param usage The image usage.
		 * @param format The image's pixel format.
		 * @param multisampleCount The multisample count of the image. Default is one.
		 * @param pDataStore The data store pointer to load the data from. Default is nullptr.
		 * @return The created texture pointer.
		 */
		[[nodiscard]] virtual std::shared_ptr<Texture2D> createTexture2D(uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format, uint32_t mipLevels = 0, Multisample multisampleCount = Multisample::One, const std::byte* pDataStore = nullptr) = 0;

		/**
		 * Get the best depth pixel format.
		 *
		 * @return The pixel format.
		 */
		[[nodiscard]] virtual PixelFormat getBestDepthFormat() const = 0;

		/**
		 * Get the max supported multisample count.
		 *
		 * @return The multisample count.
		 */
		[[nodiscard]] virtual Multisample getMaximumMultisample() const = 0;

		/**
		 * Get the instance.
		 *
		 * @return The instance reference.
		 */
		[[nodiscard]] Instance& getInstance() { return *m_pInstance; }

		/**
		 * Get the instance.
		 *
		 * @return The const instance reference.
		 */
		[[nodiscard]] const Instance& getInstance() const { return *m_pInstance; }

	private:
		std::shared_ptr<Instance> m_pInstance = nullptr;
	};
}