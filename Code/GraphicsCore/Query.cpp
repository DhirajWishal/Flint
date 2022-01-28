// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.GraphicsCore.Query;

import Flint.Core.DataType;
import Flint.GraphicsCore.DeviceBoundObject;

export namespace Flint
{
	/**
	 * Query data mode enum.
	 * These flags define how to get the data.
	 */
	enum class QueryDataMode : uint8 {
		UI64Result = BitShiftLeft(0),
		WaitForResult = BitShiftLeft(1),
		Availability = BitShiftLeft(2),
		PartialData = BitShiftLeft(3)
	};

	constexpr bool operator&(const QueryDataMode lhs, const QueryDataMode rhs) { return static_cast<uint8>(lhs) & static_cast<uint8>(rhs); }
	constexpr QueryDataMode operator|(const QueryDataMode lhs, const QueryDataMode rhs) { return static_cast<QueryDataMode>(static_cast<uint8>(lhs) | static_cast<uint8>(rhs)); }

	/**
	 * Flint query object.
	 * Queries are used to store information about a particular task in the device.
	 *
	 * Query objects can contain multiple query primitives. In Vulkan, this object contains a VkQueryPool.
	 */
	class Query : public DeviceBoundObject
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param usage The query usage.
		 * @param queryCount The number of queries this contains.
		 */
		Query(const std::shared_ptr<Device>& pDevice, const QueryUsage usage, const uint32 queryCount) : DeviceBoundObject(pDevice), mUsage(usage), mQueryCount(queryCount) {}

		/**
		 * Recreate the query with primitive count.
		 * If the current size matches the old size, it will not do anything.
		 *
		 * @param queryCount The number of query primitives required.
		 */
		virtual void Recreate(const uint32 queryCount) = 0;

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
		virtual void RequestQueryData(const uint32 firstQuery, const uint32 count, const uint64 dataSize, void* pDataStore, const uint64 stride, const QueryDataMode dataMode) = 0;

	public:
		/**
		 * Get the query usage.
		 *
		 * @return The usage enum.
		 */
		QueryUsage GetUsage() const { return mUsage; }

		/**
		 * Get the number of primitives in the query.
		 *
		 * @return The primitive count.
		 */
		uint32 GetQueryCount() const { return mQueryCount; }

	protected:
		QueryUsage mUsage = QueryUsage::Occlusion;
		uint32 mQueryCount = 0;
	};
}