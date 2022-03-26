// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Query.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanQuery final : public Query
		{
		public:
			VulkanQuery(const std::shared_ptr<Device>& pDevice, const QueryUsage usage, const uint32 queryCount);
			~VulkanQuery() { if (!bIsTerminated) Terminate(); }

			virtual void Recreate(const uint32 queryCount) override;
			virtual void RequestQueryData(const uint32 firstQuery, const uint32 count, const uint64 dataSize, void* pDataStore, const uint64 stride, const QueryDataMode dataMode) override;
			virtual void Terminate() override;

			const VkQueryPool GetQuery() const { return vQueryPool; }

		private:
			void CreatePool();

		private:
			VkQueryPool vQueryPool = VK_NULL_HANDLE;
		};
	}
}