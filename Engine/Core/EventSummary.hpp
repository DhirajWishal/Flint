// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Containers/Bitset.hpp"

namespace Flint
{
	/**
	 * Key code enum.
	 */
	enum class KeyCode : uint8_t
	{
		Unknown,

		Space,
		Apostrophe,

		Comma, Minus, Period, Slash,

		KeyZero, KeyOne, KeyTwo, KeyThree, KeyFour,
		KeyFive, KeySix, KeySeven, KeyEight, KeyNine,

		Semicolon,
		Equal,

		KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ, KeyK, KeyL, KeyM,
		KeyN, KeyO, KeyP, KeyQ, KeyR, KeyS, KeyT, KeyU, KeyV, KeyW, KeyX, KeyY, KeyZ,

		LeftBracket, RightBracket,

		Backslash,
		GraveAccent,
		WordOne, WordTwo,

		Escape,
		Enter,
		Tab,
		Backspace,
		Insert,
		Delete,
		Right,
		Left,
		Down,
		Up,
		PageUp,
		PageDown,
		Home,
		End,
		CapsLock,
		ScrollLock,
		NumLock,
		PrintScreen,
		Pause,

		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13,
		F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,

		KeyPadZero, KeyPadOne, KeyPadTwo, KeyPadThree, KeyPadFour,
		KeyPadFive, KeyPadSix, KeyPadSeven, KeyPadEight, KeyPadNine,

		KeyPadDecimal,
		KeyPadDivide,
		KeyPadMultiply,
		KeyPadSubtract,
		KeyPadAdd,
		KeyPadEqual,
		KeyPadEnter,

		LeftShift, LeftControl, LeftAlt, LeftSuper,
		RightShift, RightControl, RightAlt, RightSuper,
		Menu,

		Max
	};

	/**
	 * Special characters enum.
	 */
	enum class SpecialCharacter : uint16_t
	{
		LeftShift = 1 << 0,
		RightShift = 1 << 1,
		shift = LeftShift | RightShift,

		LeftControl = 1 << 2,
		RightControl = 1 << 3,
		Control = LeftControl | RightControl,

		LeftAlt = 1 << 4,
		RightAlt = 1 << 5,
		Alt = LeftAlt | RightAlt,

		LeftSuper = 1 << 6,
		RightSuper = 1 << 7,
		Super = LeftSuper | RightSuper,

		CapsLock = 1 << 8,
		NumLock = 1 << 9
	};

	/**
	 * Mouse button enum.
	 */
	enum class MouseButton : uint8_t
	{
		ButtonOne, ButtonTwo, ButtonThree, ButtonFour,
		ButtonFive, ButtonSix, ButtonSeven, ButtonEight,

		Last, Left,
		Right, Middle,

		Max
	};

	/**
	 * Event summary structure.
	 * This contains a summary of all the events occurred within a given frame update.
	 */
	struct EventSummary final
	{
		friend class Window;

		using KeyCodeType = std::underlying_type_t<KeyCode>;
		using MoudeButtonType = std::underlying_type_t<MouseButton>;
		using SpecialCharacterType = std::underlying_type_t<SpecialCharacter>;

		/**
		 * Check if key is pressed.
		 *
		 * @param code The key code to check.
		 * @return Whether or not the key is pressed.
		 */
		[[nodiscard]] bool isPressed(KeyCode code) const { return m_KeyCodes.test(static_cast<KeyCodeType>(code)); }

		/**
		 * Check if mouse button is pressed.
		 *
		 * @param button The mouse button to check.
		 * @return Whether or not the button is pressed.
		 */
		[[nodiscard]] bool isPressed(MouseButton button) const { return m_KeyCodes.test(static_cast<MoudeButtonType>(button)); }

		/**
		 * Check if a special character is pressed.
		 *
		 * @param character The special character(s) to check.
		 * @return Whether or not it's pressed.
		 */
		[[nodiscard]] bool isPressed(SpecialCharacter character) const { return m_SpecialCharacter & static_cast<SpecialCharacterType>(character); }

	private:
		/**
		 * Toggle key code to true indicating that the key is pressed.
		 *
		 * @param code The key code.
		 */
		void pressed(KeyCode code) { m_KeyCodes.toggleTrue(static_cast<KeyCodeType>(code)); }

		/**
		 * Toggle key code to false indicating that the key is released.
		 *
		 * @param code The key code.
		 */
		void released(KeyCode code) { m_KeyCodes.toggleFalse(static_cast<KeyCodeType>(code)); }

		/**
		 * Toggle mouse button to true indicating that the mouse button is pressed.
		 *
		 * @param button The mouse button.
		 */
		void pressed(MouseButton button) { m_MouseButtons.toggleTrue(static_cast<MoudeButtonType>(button)); }

		/**
		 * Toggle mouse button to false indicating that the mouse button is released.
		 *
		 * @param button The mouse button.
		 */
		void released(MouseButton button) { m_MouseButtons.toggleFalse(static_cast<MoudeButtonType>(button)); }

		/**
		 * Toggle special character code to true indicating that the special key is pressed.
		 *
		 * @param character The special character(s).
		 */
		void pressed(SpecialCharacter character) { m_SpecialCharacter |= static_cast<SpecialCharacterType>(character); }

		/**
		 * Toggle special character code to false indicating that the special key is released.
		 *
		 * @param character The special character(s).
		 */
		void released(SpecialCharacter character) { m_SpecialCharacter &= ~static_cast<SpecialCharacterType>(character); }

	private:
		Bitset<static_cast<KeyCodeType>(KeyCode::Max)> m_KeyCodes = {};
		Bitset<static_cast<MoudeButtonType>(MouseButton::Max)> m_MouseButtons = {};
		SpecialCharacterType m_SpecialCharacter = 0;
	};
}