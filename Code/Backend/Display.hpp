// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

#include <functional>

namespace Flint
{
	/**
	 * Event action enum.
	 */
	enum class EventAction : UI8 {
		RELEASED,
		PRESSED,
		ON_REPEAT
	};

	/**
	 * Key code enum.
	 */
	enum class KeyCode : UI8 {
		UNKNOWN,

		SPACE,
		APOSTROPHE,

		COMMA, MINUS, PERIOD, SLASH,

		KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
		KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,

		SEMICOLON,
		EQUAL,

		KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L,
		KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

		LEFT_BRACKET, RIGHT_BRACKET,

		BACKSLASH,
		GRAVE_ACCENT,
		WORLD_1, WORLD_2,

		ESCAPE,
		ENTER,
		TAB,
		BACKSPACE,
		INSERT,
		DELETE,
		RIGHT,
		LEFT,
		DOWN,
		UP,
		PAGE_UP,
		PAGE_DOWN,
		HOME,
		END,
		CAPS_LOCK,
		SCROLL_LOCK,
		NUM_LOCK,
		PRINT_SCREEN,
		PAUSE,

		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13,
		F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,

		KP_0, KP_1, KP_2, KP_3, KP_4,
		KP_5, KP_6, KP_7, KP_8, KP_9,

		KP_DECIMAL,
		KP_DIVIDE,
		KP_MULTIPLY,
		KP_SUBTRACT,
		KP_ADD,
		KP_EQUAL,
		KP_ENTER,

		LEFT_SHIFT, LEFT_CONTROL, LEFT_ALT, LEFT_SUPER,
		RIGHT_SHIFT, RIGHT_CONTROL, RIGHT_ALT, RIGHT_SUPER,
		MENU,

		KEY_CODE_MAX
	};

	/**
	 * Special characters enum.
	 */
	enum class SpecialCharacter : UI8 {
		SHIFT = BIT_SHIFT(0),
		CONTROL = BIT_SHIFT(1),
		ALT = BIT_SHIFT(2),
		SUPER = BIT_SHIFT(3),
		CAPS_LOCK = BIT_SHIFT(4),
		NUM_LOCK = BIT_SHIFT(5)
	};

	constexpr SpecialCharacter operator|(const SpecialCharacter& lhs, const SpecialCharacter& rhs) { return SpecialCharacter(static_cast<UI8>(lhs) | static_cast<UI8>(rhs)); }
	constexpr SpecialCharacter operator&(const SpecialCharacter& lhs, const SpecialCharacter& rhs) { return SpecialCharacter(static_cast<UI8>(lhs) & static_cast<UI8>(rhs)); }

	/**
	 * Mouse button enum.
	 */
	enum class MouseButton : UI8 {
		BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4,
		BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8,

		LAST, LEFT,
		RIGHT, MIDDLE,

		MOUSE_BUTTON_MAX
	};

	/**
	 * Button event structure.
	 * This structure contains information regarding a single button input.
	 */
	struct ButtonEvent
	{
		/**
		 * Default constructor.
		 */
		ButtonEvent() = default;

		/**
		 * Construct the button event using its action and special character.
		 *
		 * @param action: The action of the event.
		 * @param special: The special character of the event.
		 */
		ButtonEvent(EventAction action, SpecialCharacter special) : mAction(action), mSpecialCharacter(special) {}

		/**
		 * Activate the event.
		 *
		 * @param action: The action of the event.
		 * @param special: The special character of th event.
		 */
		void Activate(EventAction action, SpecialCharacter special) { mAction = action, mSpecialCharacter = special; }

		/**
		 * Reset the event back to its defaults.
		 */
		void Reset() { mAction = EventAction::RELEASED, mSpecialCharacter = SpecialCharacter(0); }

		/**
		 * Get the event action.
		 *
		 * @return The event action.
		 */
		const EventAction GetEventAction() const { return mAction; }

		/**
		 * Get the special character of the event.
		 *
		 * @return The special character.
		 */
		const SpecialCharacter GetSpecialCharacter() const { return mSpecialCharacter; }

		/**
		 * Check if the event is released.
		 *
		 * @return Boolean value.
		 */
		const bool IsReleased() const { return mAction == EventAction::RELEASED; }

		/**
		 * Check if the event is pressed.
		 *
		 * @return Boolean value.
		 */
		const bool IsPressed() const { return mAction == EventAction::PRESSED; }

		/**
		 * Check if the event is on repeat.
		 *
		 * @return Boolean value.
		 */
		const bool IsOnRepeat() const { return mAction == EventAction::ON_REPEAT; }

		EventAction mAction = EventAction::RELEASED;
		SpecialCharacter mSpecialCharacter = SpecialCharacter(0);
	};

	/**
	 * Flint display object.
	 * This object is responsible for a single display/ window instance.
	 */
	class Display : public FObject
	{
		friend Instance;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pInstance: The instance pointer.
		 * @param extent: The display extent. If set to (0, 0), the window will be opened in full screen mode.
		 * @param title: The display title.
		 */
		Display(const std::shared_ptr<Instance>& pInstance, const FBox2D& extent, const std::string& title);

		/**
		 * Update the display object.
		 * This polls inputs and prepares the display for the next draw.
		 */
		virtual void Update() = 0;

		/**
		 * Find the best buffer count for the swap chain.
		 * If 0 is passed (by default), the minimum supported buffer count is given. If UI32_MAX is give, the maximum supported is given.
		 *
		 * @param device: The device object to check with.
		 * @param count: The count that needs to be checked. Default is 0.
		 * @return The best buffer count integer.
		 */
		virtual UI32 FindBestBufferCount(const std::shared_ptr<Device>& pDevice, UI32 count = 0) = 0;

	protected:
		/**
		 * Terminate the device object.
		 */
		virtual void Terminate() = 0;

		/**
		 * Toggle the resize boolean to false.
		 */
		virtual void ToggleResize() = 0;

	public:
		/**
		 * Check if drag and drop paths exist.
		 *
		 * @return Boolean value.
		 */
		const bool ContainsDragAndDrop() const { return mDragAndDropPaths.empty(); }

		/**
		 * Get the drag and drop values.
		 * Once you get this value, it will automatically clear itself from the display.
		 *
		 * @return The vector of paths.
		 */
		const std::vector<std::filesystem::path> GetDragAndDropValues();

		/**
		 * Get the title of the display.
		 *
		 * @return The title string.
		 */
		const std::string GetTitle() const { return mTitle; }

		/**
		 * Set a new window title.
		 *
		 * @param title: The window title to set.
		 */
		virtual void SetTitle(const std::string& title) = 0;

		/**
		 * Set a new extent to the window.
		 *
		 * @param newExtent: The new extent to set.
		 */
		virtual void SetExtent(FBox2D newExtent) = 0;

		/**
		 * Get the display's current extent.
		 *
		 * @return The extent.
		 */
		const FBox2D GetExtent() const { return mExtent; }

		/**
		 * Get a key event from the event store.
		 *
		 * @param key: The key code.
		 * @return The button event.
		 */
		const ButtonEvent GetKeyEvent(KeyCode key) const { return mKeyEvents[static_cast<UI8>(key)]; }

		/**
		 * Get a mouse button event from the event store.
		 *
		 * @param button: The mouse button code.
		 * @return The button event.
		 */
		const ButtonEvent GetMouseButtonEvent(MouseButton button) const { return mMouseButtonEvents[static_cast<UI8>(button)]; }

		/**
		 * Get the mouse position.
		 *
		 * @return The position.
		 */
		const FExtent2D<float> GetMousePosition() const { return FExtent2D<float>(mMousePositionX, mMousePositionY); }

		/**
		 * Get the mouse scroll.
		 *
		 * @return The mouse scroll as (up, down).
		 */
		const FExtent2D<float> GetMouseScroll() const { return FExtent2D<float>(mMouseScrollUp, mMouseScrollDown); }

		/**
		 * Check if the display is open.
		 *
		 * @return Boolean value.
		 */
		const bool IsOpen() const { return mIsDisplayOpen; }

		/**
		 * Check if the display is resized.
		 *
		 * @return Boolean value.
		 */
		const bool IsDisplayResized() const { return mIsDislayResized; }

		/**
		 * Check if the cursor is within the display.
		 *
		 * @return Boolean value.
		 */
		const bool IsCursorWithinDisplay() const { return mIsCursorWithinDisplay; }

	protected:
		std::vector<std::filesystem::path> mDragAndDropPaths;

		std::string mTitle = "";
		std::shared_ptr<Instance> pInstance = nullptr;
		FBox2D mExtent = {};

		ButtonEvent mKeyEvents[static_cast<UI8>(KeyCode::KEY_CODE_MAX)] = {};
		ButtonEvent mMouseButtonEvents[static_cast<UI8>(MouseButton::MOUSE_BUTTON_MAX)] = {};

		float mMousePositionX = 0.0f;
		float mMousePositionY = 0.0f;

		float mMouseScrollUp = 0.0f;
		float mMouseScrollDown = 0.0f;

		bool mIsDisplayOpen = true;
		bool mIsDislayResized = false;
		bool mIsCursorWithinDisplay = false;
	};
}