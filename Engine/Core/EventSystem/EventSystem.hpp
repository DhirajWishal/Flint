// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Keyboard.hpp"
#include "Mouse.hpp"

namespace Flint
{
	/**
	 * Event type enum.
	 */
	enum class EventType : uint8_t
	{
		None,
		Keyboard,
		Mouse,
		Close
	};

	/**
	 * Event system class.
	 * This contains all the events. To update, call the poll() function and then all the events can be accessed.
	 *
	 * Note that the application needs just one instance of this, and the poll method should be called after creating a Window.
	 */
	class EventSystem final
	{
	public:
		/**
		 * Default constructor.
		 */
		constexpr EventSystem() = default;

		/**
		 * Poll all the events.
		 *
		 * @return Enum stating the type of event.
		 */
		[[nodiscard]] EventType poll();

		/**
		 * Get the keyboard from the system.
		 *
		 * @return The keyboard.
		 */
		[[nodiscard]] const Keyboard& keyboard() const { return m_Keyboard; }

		/**
		 * Get the mouse from the system.
		 *
		 * @return The mouse.
		 */
		[[nodiscard]] const Mouse& mouse() const { return m_Mouse; }

		/**
		 * Check if the application should close.
		 * This occurs if the user clicks on the close button, or if the application terminates.
		 *
		 * @return Whether or not if the application should close.
		 */
		[[nodiscard]] bool shouldClose() const { return m_ShouldClose; }

	private:
		/**
		 * Handle the key event.
		 *
		 * @param scancode The scancode of the key.
		 * @param pressed If the key is pressed or not.
		 */
		void handleKeyEvent(int32_t scancode, bool pressed);

		/**
		 * Handle the special characters.
		 *
		 * @param characters The special characters.
		 * @param pressed If the key is pressed or not.
		 */
		void handleSpecialCharacter(uint64_t characters, bool pressed);

		/**
		 * Handle mouse button event.
		 *
		 * @param button The button number.
		 * @param clicks Number of times the button was clicked.
		 */
		void handleMouseButtonEvent(uint8_t button, uint8_t clicks);

	private:
		Keyboard m_Keyboard = {};
		Mouse m_Mouse = {};

		bool m_ShouldClose = false;
	};
}