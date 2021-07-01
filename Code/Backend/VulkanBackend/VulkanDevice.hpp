// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"
#include "VulkanInstance.hpp"
#include "VulkanQueue.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice final : public Device, public std::enable_shared_from_this<VulkanDevice>
		{
		public:
			VulkanDevice(const std::shared_ptr<Instance>& pInstance, DeviceFlags flags);

			virtual bool IsDisplayCompatible(const const std::shared_ptr<Display>& pDisplay) override final;

			virtual std::shared_ptr<CommandBufferList> CreatePrimaryCommandBufferList(UI32 bufferCount) override final;
			virtual std::shared_ptr<CommandBufferList> CreateSecondaryCommandBufferList(UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pParent) override final;
			virtual void DestroyCommandBufferList(const std::shared_ptr<CommandBufferList>& pCommandBufferList) override final;

			virtual std::shared_ptr<ScreenBoundRenderTarget> CreateScreenBoundRenderTarget(const std::shared_ptr<Display>& pDisplay, const FExtent2D& extent, const UI32 bufferCount) override final;
			virtual void DestroyRenderTarget(const std::shared_ptr<RenderTarget>& pRenderTarget) override final;

			virtual std::shared_ptr<Buffer> CreateBuffer(BufferType type, UI64 size) override final;
			virtual void DestroyBuffer(const std::shared_ptr<Buffer>& pBuffer) override final;

			virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::filesystem::path& path, ShaderCodeType codeType) override final;
			virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::vector<UI32>& code, ShaderCodeType codeType = ShaderCodeType::SPIR_V) override final;
			virtual std::shared_ptr<Shader> CreateShader(ShaderType type, const std::string& code, ShaderCodeType codeType = ShaderCodeType::GLSL) override final;
			virtual void DestroyShader(const std::shared_ptr<Shader>& pShader) override final;

			virtual void WaitIdle() override final;
			virtual void WaitForQueue() override final;

			virtual void Terminate() override final;

		public:
			VkDevice GetLogicalDevice() const noexcept { return vLogicalDevice; }
			VkPhysicalDevice GetPhysicalDevice() const noexcept { return vPhysicalDevice; }

			VulkanQueue& GetQueue() { return vQueue; }
			const VulkanQueue GetQueue() const { return vQueue; }
			VkSampleCountFlags GetSampleCount() const { return vSampleCount; }

			VkResult CreateImageMemory(const std::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const;
			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const;
			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const;

		private:
			void InitializePhysicalDevice();

			void InitializeLogicalDevice();
			void TerminateLogicalDevice();

		private:
			VulkanQueue vQueue = {};

			std::vector<const char*> mDeviceExtensions;

			VkDevice vLogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;

			VkSampleCountFlags vSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;
		};
	}
}