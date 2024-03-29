// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Backend/Device.hpp"
#include "Flint/Backend/Types.hpp"
#include "Flint/Core/Containers/SparseArray.hpp"
#include "VulkanInstance.hpp"

#include <vk_mem_alloc.h>

#include <unordered_map>

namespace Flint
{
	namespace Backend
	{
		class VulkanTextureSampler;

		/**
		 * Vulkan queue structure.
		 * This contains all the information about a single queue.
		 */
		struct VulkanQueue final
		{
			VkQueue m_Queue = VK_NULL_HANDLE;
			uint32_t m_Family = 0;
		};

		/**
		 * Vulkan engine class.
		 */
		class VulkanDevice final : public std::enable_shared_from_this<VulkanDevice>, public Device
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pInstance The instance used to create the object.
			 */
			explicit VulkanDevice(const std::shared_ptr<VulkanInstance>& pInstance);

			/**
			 * Destructor.
			 */
			~VulkanDevice() override;

			/**
			 * Create a new buffer.
			 *
			 * @param size The size of the buffer.
			 * @param usage The buffer usage.
			 * @param pDataStore The data store pointer to copy everything from. Make sure that the raw buffer's size is the same or more than the buffer's size. Default is nullptr.
			 * @return The buffer pointer.
			 */
			[[nodiscrad]] std::shared_ptr<Buffer> createBuffer(uint64_t size, BufferUsage usage, const std::byte* pDataStore = nullptr) override;

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
			[[nodiscard]] std::shared_ptr<Rasterizer> createRasterizer(Camera& camera, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One, bool exclusiveBuffering = false) override;

			/**
			 * Create a new ray tracer.
			 *
			 * @param camera The camera reference which is used to generate the images using.
			 * @param frameCount The number of frames to use.
			 * @return The ray tracer pointer.
			 */
			[[nodiscard]] std::shared_ptr<RayTracer> createRayTracer(Camera& camera, uint32_t frameCount) override;

			/**
			 * Create a new window.
			 *
			 * @param title The window title.
			 * @param width The width of the window. If set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
			 * @param height The height of the window. Same as the width, if set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
			 * @return The window pointer.
			 */
			[[nodiscard]] std::shared_ptr<Window> createWindow(std::string&& title, uint32_t width = -1, uint32_t height = -1) override;

			/**
			 * Create a new rasterizing program.
			 *
			 * @param vertexShader The vertex shader code.
			 * @param fragmentShader The fragment shader code.
			 * @return The rasterizing program pointer.
			 */
			[[nodiscard]] std::shared_ptr<RasterizingProgram> createRasterizingProgram(ShaderCode&& vertexShader, ShaderCode&& fragementShader) override;

			/**
			 * Create a new static model.
			 *
			 * @param assetFile The asset file to load the data from.
			 * @return The loaded static model.
			 */
			[[nodiscard]] std::shared_ptr<StaticModel> createStaticModel(std::filesystem::path&& assetFile) override;

			/**
			 * Create a new 2D texture image.
			 *
			 * @param width The width of the image.
			 * @param height The height of the image.
			 * @param usage The image usage.
			 * @param format The image's pixel format.
			 * @param mipLevels The texture's mip levels. If this is set to 0 (default), the best value is set.
			 * @param multisampleCount The multisample count of the image. Default is one.
			 * @param pDataStore The data store pointer to load the data from. Default is nullptr.
			 * @return The created texture pointer.
			 */
			[[nodiscard]] std::shared_ptr<Texture2D> createTexture2D(uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format, uint32_t mipLevels = 0, Multisample multisampleCount = Multisample::One, const std::byte* pDataStore = nullptr) override;

			/**
			 * Create a new texture sampler.
			 * Note that we are caching this to make sure that we don't waste resources.
			 *
			 * @param specification The sampler specification.
			 * @return The sampler pointer.
			 */
			[[nodiscard]] std::shared_ptr<TextureSampler> createTextureSampler(TextureSamplerSpecification&& specification) override;

			/**
			 * Create new command buffers.
			 *
			 * @param bufferCount The command buffer count. Default is 1.
			 */
			[[nodiscard]] std::shared_ptr<CommandBuffers> createCommandBuffers(uint32_t bufferCount = 1) override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

			/**
			 * Wait till all the commands have finished execution.
			 */
			void waitIdle() override;

			/**
			 * Get the best depth pixel format.
			 *
			 * @return The pixel format.
			 */
			[[nodiscard]] PixelFormat getBestDepthFormat() const override;

			/**
			 * Get the max supported multisample count.
			 *
			 * @return The multisample count.
			 */
			[[nodiscard]] Multisample getMaximumMultisample() const override;

		public:
			/**
			 * Get the physical device properties.
			 *
			 * @return The physical device properties.
			 */
			[[nodiscard]] const VkPhysicalDeviceProperties& getPhysicalDeviceProperties() const { return m_PhysicalDeviceProperties; }

			/**
			 * Get the Vulkan functions from the internal device table.
			 *
			 * @return The device table.
			 */
			[[nodiscard]] const VolkDeviceTable& getDeviceTable() const { return m_DeviceTable; }

			/**
			 * Get the Vulkan physical deice.
			 *
			 * @return The Vulkan physical device.
			 */
			[[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }

			/**
			 * Get the Vulkan logical device.
			 *
			 * @return The Vulkan logical device.
			 */
			[[nodiscard]] VkDevice getLogicalDevice() const { return m_LogicalDevice; }

			/**
			 * Get the graphics queue from the engine.
			 *
			 * @return The graphics queue.
			 */
			[[nodiscard]] Synchronized<VulkanQueue>& getGraphicsQueue() { return m_GraphicsQueue; }

			/**
			 * Get the graphics queue from the engine.
			 *
			 * @return The graphics queue.
			 */
			[[nodiscard]] const Synchronized<VulkanQueue>& getGraphicsQueue() const { return m_GraphicsQueue; }

			/**
			 * Get the compute queue from the engine.
			 *
			 * @return The compute queue.
			 */
			[[nodiscard]] Synchronized<VulkanQueue>& getComputeQueue() { return m_ComputeQueue; }

			/**
			 * Get the compute queue from the engine.
			 *
			 * @return The compute queue.
			 */
			[[nodiscard]] const Synchronized<VulkanQueue>& getComputeQueue() const { return m_ComputeQueue; }

			/**
			 * Get the transfer queue from the engine.
			 *
			 * @return The transfer queue.
			 */
			[[nodiscard]] Synchronized<VulkanQueue>& getTransferQueue() { return m_TransferQueue; }

			/**
			 * Get the transfer queue from the engine.
			 *
			 * @return The transfer queue.
			 */
			[[nodiscard]] const Synchronized<VulkanQueue>& getTransferQueue() const { return m_TransferQueue; }

			/**
			 * Get the VMA allocator.
			 *
			 * @return The allocator.
			 */
			[[nodiscard]] Synchronized<VmaAllocator>& getAllocator() { return m_Allocator; }

			/**
			 * Get the VMA allocator.
			 *
			 * @return The allocator.
			 */
			[[nodiscard]] const Synchronized<VmaAllocator>& getAllocator() const { return m_Allocator; }

		private:
			/**
			 * Select the best physical device for the engine.
			 */
			void selectPhysicalDevice();

			/**
			 * Create a new logical device.
			 */
			void createLogicalDevice();

			/**
			 * Destroy the created logical device.
			 */
			void destroyLogicalDevice();

			/**
			 * Create the VMA allocator.
			 */
			void createVMAAllocator();

			/**
			 * Destroy the VMA allocator.
			 */
			void destroyVMAAllocator();

		private:
			std::unordered_map<uint64_t, std::shared_ptr<VulkanTextureSampler>> m_Samplers;

			VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};

			VolkDeviceTable m_DeviceTable = {};

			std::vector<const char*> m_DeviceExtensions;

			Synchronized<VulkanQueue> m_GraphicsQueue;
			Synchronized<VulkanQueue> m_ComputeQueue;
			Synchronized<VulkanQueue> m_TransferQueue;

			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;

			Synchronized<VmaAllocator> m_Allocator = nullptr;
		};

		namespace Utility
		{
			/**
			 * Get the Vulkan sample count flag bits from multisample.
			 *
			 * @param multisample The multisample count.
			 * @return The Vulkan sample count.
			 */
			[[nodiscard]] VkSampleCountFlagBits GetSampleCountFlagBits(Multisample multisample);

			/**
			 * Get the Vulkan image format from pixel format.
			 *
			 * @param format The pixel format.
			 * @return The Vulkan format.
			 */
			[[nodiscard]] VkFormat GetImageFormat(PixelFormat format);

			/**
			 * Get the pixel format from the Vulkan image format.
			 *
			 * @param format The Vulkan format.
			 * @return The pixel format.
			 */
			[[nodiscard]] PixelFormat GetPixelFormat(VkFormat format);

			/**
			 * Get the pipeline stage flags from access flags.
			 *
			 * @param flags Access flags.
			 * @return The stage flags.
			 */
			[[nodiscard]] VkPipelineStageFlags GetPipelineStageFlags(VkAccessFlags flags);

			/**
			 * Find the best supported format available.
			 *
			 * @param device The Vulkan engine.
			 * @param candidates The format candidates.
			 * @param tiling The image tiling.
			 * @param features The format features.
			 *
			 */
			[[nodiscard]] VkFormat FindSupportedFormat(const VulkanDevice* pDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

			/**
			 * Check if a format has the stencil component.
			 *
			 * @param format The format to check.
			 */
			[[nodiscard]] bool HasStencilComponent(VkFormat format);

			/**
			 * Find the best depth format.
			 *
			 * @param device The Vulkan engine.
			 */
			[[nodiscard]] VkFormat FindDepthFormat(const VulkanDevice* pDevice);

			/**
			 * Get the descriptor type.
			 *
			 * @param type The flint descriptor type.
			 * @return The Vulkan descriptor type.
			 */
			[[nodiscard]] VkDescriptorType GetDescriptorType(ResourceType type);

			/**
			 * Get the size from a format.
			 *
			 * @param format The format to get the size of.
			 * @return The byte size.
			 */
			[[nodiscard]] uint8_t GetSizeFromFormat(VkFormat format);

			/**
			 * Get the format from the attribute type.
			 *
			 * @param type The type of the attribute.
			 * @return The Vulkan format.
			 */
			[[nodiscard]] VkFormat GetVkFormat(Flint::DataType type);
		}
	}
}