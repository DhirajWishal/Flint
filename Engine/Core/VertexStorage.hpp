// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Vertex memory type enum.
	 * This defines how to store the vertex data in a vertex storage.
	 */
	enum class VertexMemoryType : uint8_t
	{
		// This means that each and every vertex attribute gets it's own vertex buffer.
		Exclusive,

		// This means that each and every vertex attribute gets stored in a single interleaved buffer. 
		Interleaved
	};

	/**
	 * Vertex storage class.
	 * This class is used to store vertex information about a single geometry.
	 */
	class VertexStorage : public DeviceBoundObject
	{
	public:
		// Using the same constructor as the super class.
		using DeviceBoundObject::DeviceBoundObject;

		/**
		 * Default virtual destructor.
		 */
		virtual ~VertexStorage() = default;

		/**
		 * Insert data to the vertex storage.
		 *
		 * @param attribute The attribute type of the data.
		 * @param pStaggingBuffer The stagging buffer pointer to copy the data from.
		 * @return The offset at which the newly inserted data are stored.
		 */
		virtual [[nodiscard]] uint64_t insert(VertexAttribute attribute, const Buffer* pStaggingBuffer) = 0;

	protected:
		const VertexMemoryType m_MemoryType = VertexMemoryType::Exclusive;
	};
}