// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Core/Containers/Bytes.hpp"

namespace Flint
{
	void Bytes::insert(std::string_view string)
	{
		//for (const char character : string)
		//	m_Bytes.emplace_back(character);
	}

	void Bytes::insert(uint64_t value)
	{
		// m_Bytes.emplace_back(value >> 56);
		// m_Bytes.emplace_back(value >> 48);
		// m_Bytes.emplace_back(value >> 40);
		// m_Bytes.emplace_back(value >> 32);
		// m_Bytes.emplace_back(value >> 24);
		// m_Bytes.emplace_back(value >> 16);
		// m_Bytes.emplace_back(value >> 8);
		// m_Bytes.emplace_back(value >> 0);
	}

}