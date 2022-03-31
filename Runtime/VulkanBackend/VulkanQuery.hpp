// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Query.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanQuery final : public Query<VulkanDevice>
		{
		public:
			explicit VulkanQuery(VulkanDevice* pDevice, const QueryUsage usage, const uint32_t queryCount);
			~VulkanQuery() { if (!bIsTerminated) Terminate(); }

			virtual void Recreate(const uint32_t queryCount) override;
			virtual void RequestQueryData(const uint32_t firstQuery, const uint32_t count, const uint64_t dataSize, void* pDataStore, const uint64_t stride, const QueryDataMode dataMode) override;
			virtual void Terminate() override;

			const VkQueryPool GetQuery() const { return m_vQueryPool; }

		private:
			void CreatePool();

		private:
			VkQueryPool m_vQueryPool = VK_NULL_HANDLE;
		};
	}
}