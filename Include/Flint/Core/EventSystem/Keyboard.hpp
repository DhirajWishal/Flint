// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace Flint
{
	/**
	 * Keyboard structure.
	 * This contains all the keyboard inputs and can be checked if pressed by seeing if the required key is true (pressed) or not.
	 */
	struct Keyboard final
	{
		std::string m_TextInput;

		bool m_Unknown : 1;

		bool m_Space : 1;
		bool m_Apostrophe : 1;

		bool m_Comma : 1;
		bool m_Minus : 1;
		bool m_Period : 1;
		bool m_Slash : 1;

		bool m_KeyZero : 1;
		bool m_KeyOne : 1;
		bool m_KeyTwo : 1;
		bool m_KeyThree : 1;
		bool m_KeyFour : 1;
		bool m_KeyFive : 1;
		bool m_KeySix : 1;
		bool m_KeySeven : 1;
		bool m_KeyEight : 1;
		bool m_KeyNine : 1;

		bool m_Semicolon : 1;
		bool m_Equal : 1;

		bool m_KeyA : 1;
		bool m_KeyB : 1;
		bool m_KeyC : 1;
		bool m_KeyD : 1;
		bool m_KeyE : 1;
		bool m_KeyF : 1;
		bool m_KeyG : 1;
		bool m_KeyH : 1;
		bool m_KeyI : 1;
		bool m_KeyJ : 1;
		bool m_KeyK : 1;
		bool m_KeyL : 1;
		bool m_KeyM : 1;
		bool m_KeyN : 1;
		bool m_KeyO : 1;
		bool m_KeyP : 1;
		bool m_KeyQ : 1;
		bool m_KeyR : 1;
		bool m_KeyS : 1;
		bool m_KeyT : 1;
		bool m_KeyU : 1;
		bool m_KeyV : 1;
		bool m_KeyW : 1;
		bool m_KeyX : 1;
		bool m_KeyY : 1;
		bool m_KeyZ : 1;

		bool m_LeftBracket : 1;
		bool m_RightBracket : 1;

		bool m_Backslash : 1;
		bool m_GraveAccent : 1;
		bool m_WordOne : 1;
		bool m_WordTwo : 1;

		bool m_Escape : 1;
		bool m_Enter : 1;
		bool m_Tab : 1;
		bool m_Backspace : 1;
		bool m_Insert : 1;
		bool m_Delete : 1;
		bool m_Right : 1;
		bool m_Left : 1;
		bool m_Down : 1;
		bool m_Up : 1;
		bool m_PageUp : 1;
		bool m_PageDown : 1;
		bool m_Home : 1;
		bool m_End : 1;
		bool m_CapsLock : 1;
		bool m_ScrollLock : 1;
		bool m_NumLock : 1;
		bool m_PrintScreen : 1;
		bool m_Pause : 1;

		bool m_F1 : 1;
		bool m_F2 : 1;
		bool m_F3 : 1;
		bool m_F4 : 1;
		bool m_F5 : 1;
		bool m_F6 : 1;
		bool m_F7 : 1;
		bool m_F8 : 1;
		bool m_F9 : 1;
		bool m_F10 : 1;
		bool m_F11 : 1;
		bool m_F12 : 1;
		bool m_F13 : 1;
		bool m_F14 : 1;
		bool m_F15 : 1;
		bool m_F16 : 1;
		bool m_F17 : 1;
		bool m_F18 : 1;
		bool m_F19 : 1;
		bool m_F20 : 1;
		bool m_F21 : 1;
		bool m_F22 : 1;
		bool m_F23 : 1;
		bool m_F24 : 1;
		bool m_F25 : 1;

		bool m_KeyPadZero : 1;
		bool m_KeyPadOne : 1;
		bool m_KeyPadTwo : 1;
		bool m_KeyPadThree : 1;
		bool m_KeyPadFour : 1;
		bool m_KeyPadFive : 1;
		bool m_KeyPadSix : 1;
		bool m_KeyPadSeven : 1;
		bool m_KeyPadEight : 1;
		bool m_KeyPadNine : 1;

		bool m_KeyPadDecimal : 1;
		bool m_KeyPadDivide : 1;
		bool m_KeyPadMultiply : 1;
		bool m_KeyPadSubtract : 1;
		bool m_KeyPadAdd : 1;
		bool m_KeyPadEqual : 1;
		bool m_KeyPadEnter : 1;

		bool m_LeftShift : 1;
		bool m_LeftControl : 1;
		bool m_LeftAlt : 1;
		bool m_LeftSuper : 1;
		bool m_RightShift : 1;
		bool m_RightControl : 1;
		bool m_RightAlt : 1;
		bool m_RightSuper : 1;

		bool m_Menu : 1;
	};
}