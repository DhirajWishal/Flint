// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Query data mode enum.
	 * These flags define how to get the data.
	 */
	enum class QueryDataMode : uint8_t
	{
		UI64Result = BitShiftLeft(0),
		WaitForResult = BitShiftLeft(1),
		Availability = BitShiftLeft(2),
		PartialData = BitShiftLeft(3)
	};

	constexpr bool operator&(const QueryDataMode lhs, const QueryDataMode rhs) { return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs); }
	constexpr QueryDataMode operator|(const QueryDataMode lhs, const QueryDataMode rhs) { return static_cast<QueryDataMode>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)); }

	/**
	 * Flint query object.
	 * Queries are used to store information about a particular task in the device.
	 *
	 * Query objects can contain multiple query primitives. In Vulkan, this object contains a VkQueryPool.
	 */
	template<class DeviceT>
	class Query : public DeviceBoundObject<DeviceT>
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param usage The query usage.
		 * @param queryCount The number of queries this contains.
		 */
		Query(DeviceT* pDevice, const QueryUsage usage, const uint32_t queryCount) : DeviceBoundObject(pDevice), m_Usage(usage), m_QueryCount(queryCount) {}

		/**
		 * Recreate the query with primitive count.
		 * If the current size matches the old size, it will not do anything.
		 *
		 * @param queryCount The number of query primitives required.
		 */
		virtual void Recreate(const uint32_t queryCount) = 0;

		/**
		 * Request data from the query.
		 *
		 * @param firstQuery The first query to get data from.
		 * @param count The number of queries to get data from.
		 * @param dataSize The size of data that is being requested.
		 * @param pDataStore The data store pointer to store the data.
		 * @param stride The data stride.
		 * @param dataMode The data mode defining how to get the data.
		 */
		virtual void RequestQueryData(const uint32_t firstQuery, const uint32_t count, const uint64_t dataSize, void* pDataStore, const uint64_t stride, const QueryDataMode dataMode) = 0;

	public:
		/**
		 * Get the query usage.
		 *
		 * @return The usage enum.
		 */
		QueryUsage GetUsage() const { return m_Usage; }

		/**
		 * Get the number of primitives in the query.
		 *
		 * @return The primitive count.
		 */
		uint32_t GetQueryCount() const { return m_QueryCount; }

	protected:
		QueryUsage m_Usage = QueryUsage::Occlusion;
		uint32_t m_QueryCount = 0;
	};
}