// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ComputePipeline.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanComputePipeline final : public ComputePipeline, public boost::enable_shared_from_this<VulkanComputePipeline>
		{
		public:
			VulkanComputePipeline(const boost::shared_ptr<Device>& pDevice, const std::string& pipelineName, const boost::shared_ptr<Shader>& pComputeShader);
			~VulkanComputePipeline() { if (!bIsTerminated) Terminate(); }

			virtual void ReloadShaders() override final;
			virtual void Dispatch() override final;
			virtual void Dispatch(const boost::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget) override final;
			virtual void Dispatch(const boost::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget) override final;

			virtual void PrepareResources() override final;

			virtual void Terminate() override final;

			const VkPipelineLayout GetPipelineLayout() const { return vPipelineLayout; }
			const VkPipeline GetPipeline() const { return vPipeline; }
			const VkDescriptorSet GetDescriptorSet(const boost::shared_ptr<ResourceMap>& pResourceMap) const;
			const VkDescriptorSet* GetDescriptorSetAddress(const boost::shared_ptr<ResourceMap>& pResourceMap) const;

		private:
			void CreatePipelineCache();
			void CreatePipelineLayout();
			void CreatePipeline();

		private:
			boost::shared_ptr<ComputePipeline> pThisPipeline = nullptr;
			boost::unordered::unordered_map<boost::shared_ptr<ResourceMap>, VkDescriptorSet> vDescriptorSetMap;

			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineCache vPipelineCache = VK_NULL_HANDLE;

			VkDescriptorSetLayout vDescriptorSetLayout = VK_NULL_HANDLE;
			VkDescriptorPool vDescriptorSetPool = VK_NULL_HANDLE;
		};
	}
}