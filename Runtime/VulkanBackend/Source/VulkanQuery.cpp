// Copyright 2021-2022 Dhiraj Wishal
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

		VulkanQuery::VulkanQuery(VulkanDevice* m_pDevice, const QueryUsage usage, const uint32_t queryCount)
			: Query(m_pDevice, usage, queryCount)
		{
			CreatePool();
		}

		void VulkanQuery::Recreate(const uint32_t queryCount)
		{
			// Skip if the two sizes are equal.
			if (queryCount == m_QueryCount)
				return;

			m_QueryCount = queryCount;
			Terminate();
			CreatePool();

			bIsTerminated = false;
		}

		void VulkanQuery::RequestQueryData(const uint32_t firstQuery, const uint32_t count, const uint64_t dataSize, void* pDataStore, const uint64_t stride, const QueryDataMode dataMode)
		{
			VkQueryResultFlags m_vResultFlags = 0;
			if (dataMode & QueryDataMode::UI64Result)
				m_vResultFlags |= VkQueryResultFlagBits::VK_QUERY_RESULT_64_BIT;

			if (dataMode & QueryDataMode::WaitForResult)
				m_vResultFlags |= VkQueryResultFlagBits::VK_QUERY_RESULT_WAIT_BIT;

			if (dataMode & QueryDataMode::Availability)
				m_vResultFlags |= VkQueryResultFlagBits::VK_QUERY_RESULT_WITH_AVAILABILITY_BIT;

			if (dataMode & QueryDataMode::PartialData)
				m_vResultFlags |= VkQueryResultFlagBits::VK_QUERY_RESULT_PARTIAL_BIT;

			auto& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkGetQueryPoolResults(m_vDevice.GetLogicalDevice(), m_vQueryPool, firstQuery, count, dataSize, pDataStore, stride, m_vResultFlags));
		}

		void VulkanQuery::Terminate()
		{
			auto& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();
			m_vDevice.GetDeviceTable().vkDestroyQueryPool(m_vDevice.GetLogicalDevice(), m_vQueryPool, nullptr);

			bIsTerminated = true;
		}

		void VulkanQuery::CreatePool()
		{
			VkQueryPoolCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.queryCount = m_QueryCount;
			m_vCreateInfo.pipelineStatistics;
			m_vCreateInfo.queryType = Helpers::GetQueryType(m_Usage);

			auto& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateQueryPool(m_vDevice.GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vQueryPool));
		}
	}
}