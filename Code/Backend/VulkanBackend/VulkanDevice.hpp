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
		class VulkanDevice final : public Backend::Device
		{
		public:
			VulkanDevice(Backend::Instance& instance, Backend::DeviceFlags flags);

			virtual bool IsDisplayCompatible(const Backend::Display& display) override final;

			virtual Backend::CommandBufferList& CreatePrimaryCommandBufferList(UI32 bufferCount) override final;
			virtual Backend::CommandBufferList& CreateSecondaryCommandBufferList(UI32 bufferCount, Backend::CommandBufferList& parent) override final;
			virtual void DestroyCommandBufferList(Backend::CommandBufferList& commandBufferList) override final;

			virtual Backend::ScreenBoundRenderTarget& CreateScreenBoundRenderTarget(Backend::Display& display, const FExtent2D& extent, const UI32 bufferCount) override final;
			virtual void DestroyRenderTarget(Backend::RenderTarget& renderTarget) override final;

			virtual void WaitIdle() override final;
			virtual void WaitForQueue() override final;

			virtual Backend::Buffer& CreateBuffer(Backend::BufferType type, UI64 size) override final;
			virtual void DestroyBuffer(Backend::Buffer& buffer) override final;

			virtual Backend::Shader& CreateShader(Backend::ShaderType type, const std::filesystem::path& path, Backend::ShaderCodeType codeType) override final;
			virtual Backend::Shader& CreateShader(Backend::ShaderType type, const std::vector<UI32>& code, Backend::ShaderCodeType codeType = Backend::ShaderCodeType::SPIR_V) override final;
			virtual Backend::Shader& CreateShader(Backend::ShaderType type, const std::string& code, Backend::ShaderCodeType codeType = Backend::ShaderCodeType::GLSL) override final;
			virtual void DestroyShader(Backend::Shader& shader) override final;

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