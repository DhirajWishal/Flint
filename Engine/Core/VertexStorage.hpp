// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	namespace Core
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
		template<class TDevice>
		class VertexStorage : public DeviceBoundObject<TDevice>
		{
		public:
			// Using the same constructor as the super class.
			using DeviceBoundObject<TDevice>::DeviceBoundObject;

			/**
			 * Default virtual destructor.
			 */
			virtual ~VertexStorage() = default;

		protected:
			const VertexMemoryType m_MemoryType = VertexMemoryType::Exclusive;
		};
	}
}