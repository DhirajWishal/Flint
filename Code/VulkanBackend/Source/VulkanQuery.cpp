// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanQuery.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Helpers
		{
			VkQueryType GetQueryType(QueryUsage usage)
			{
				switch (usage)
				{
				case QueryUsage::Occlusion:									return VkQueryType::VK_QUERY_TYPE_OCCLUSION;
				case QueryUsage::PipelineStatistics:						return VkQueryType::VK_QUERY_TYPE_PIPELINE_STATISTICS;
				case QueryUsage::Timestamp:									return VkQueryType::VK_QUERY_TYPE_TIMESTAMP;
				case QueryUsage::TransformFeedbackStream:					return VkQueryType::VK_QUERY_TYPE_TRANSFORM_FEEDBACK_STREAM_EXT;
				case QueryUsage::Performance:								return VkQueryType::VK_QUERY_TYPE_PERFORMANCE_QUERY_KHR;
				case QueryUsage::AccelerationStructureCompactedSize:		return VkQueryType::VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
				case QueryUsage::AccelerationStructureSerializationSize:	return VkQueryType::VK_QUERY_TYPE_ACCELERATION_STRUCTURE_SERIALIZATION_SIZE_KHR;
				}

				throw backend_error("Invalid or Undefined query usage!");
			}
		}

		VulkanQuery::VulkanQuery(const std::shared_ptr<Device>& pDevice, const QueryUsage usage, const uint32 queryCount)
			: Query(pDevice, usage, queryCount)
		{
			CreatePool();
		}

		void VulkanQuery::Recreate(const uint32 queryCount)
		{
			// Skip if the two sizes are equal.
			if (queryCount == mQueryCount)
				return;

			mQueryCount = queryCount;
			Terminate();
			CreatePool();

			bIsTerminated = false;
		}

		void VulkanQuery::RequestQueryData(const uint32 firstQuery, const uint32 count, const uint64 dataSize, void* pDataStore, const uint64 stride, const QueryDataMode dataMode)
		{
			VkQueryResultFlags vResultFlags = 0;
			if (dataMode & QueryDataMode::UI64Result)
				vResultFlags |= VkQueryResultFlagBits::VK_QUERY_RESULT_64_BIT;

			if (dataMode & QueryDataMode::WaitForResult)
				vResultFlags |= VkQueryResultFlagBits::VK_QUERY_RESULT_WAIT_BIT;

			if (dataMode & QueryDataMode::Availability)
				vResultFlags |= VkQueryResultFlagBits::VK_QUERY_RESULT_WITH_AVAILABILITY_BIT;

			if (dataMode & QueryDataMode::PartialData)
				vResultFlags |= VkQueryResultFlagBits::VK_QUERY_RESULT_PARTIAL_BIT;

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkGetQueryPoolResults(vDevice.GetLogicalDevice(), vQueryPool, firstQuery, count, dataSize, pDataStore, stride, vResultFlags));
		}

		void VulkanQuery::Terminate()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkDestroyQueryPool(vDevice.GetLogicalDevice(), vQueryPool, nullptr);

			bIsTerminated = true;
		}

		void VulkanQuery::CreatePool()
		{
			VkQueryPoolCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.queryCount = mQueryCount;
			vCreateInfo.pipelineStatistics;
			vCreateInfo.queryType = Helpers::GetQueryType(mUsage);

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateQueryPool(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vQueryPool));
		}
	}
}