// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

namespace Flint
{
	/**
	 * Mouse structure.
	 * This contains all the inputs of a mouse.
	 *
	 * Buttons contain the number of clicks.
	 */
	struct Mouse final
	{
		float m_PositionX = 0.0f;
		float m_PositionY = 0.0f;

		float m_ScrollX = 0.0f;
		float m_ScrollY = 0.0f;

		uint8_t m_Left = 0;
		uint8_t m_Middle = 0;
		uint8_t m_Right = 0;
		uint8_t m_Aux1 = 0;	// Auxiliary button 1.
		uint8_t m_Aux2 = 0;	// Auxiliary button 2.
	};

}