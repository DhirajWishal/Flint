// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

#include <filesystem>
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
		MENU
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
		RIGHT, MIDDLE
	};

	/**
	 * Key callback function type.
	 *
	 * @param KeyCode: The key code of the key.
	 * @param EventAction: The key action.
	 * @param SpecialCharacter: The special characters pressed at the time.
	 */
	using TKeyCallbackFunc = std::function<void(KeyCode, EventAction, SpecialCharacter)>;

	/**
	 * Mouse button callback function type.
	 *
	 * @param MouseButton: The mouse button.
	 * @param EventAction: The button action.
	 * @param SpecialCharacter: The special characters pressed at the time.
	 */
	using TMouseButtonCallbackFunc = std::function<void(MouseButton, EventAction, SpecialCharacter)>;

	/**
	 * Cursor position callback function type.
	 *
	 * @param xOffset: The x offset of the cursor.
	 * @param yOffset: The y offset of the cursor.
	 */
	using TCursorPositionCallbackFunc = std::function<void(double, double)>;

	/**
	 * Mouse scroll callback function type.
	 *
	 * @param xOffset: The x offset.
	 * @param yOffset: The y offset.
	 */
	using TMouseScrollCallbackFunc = std::function<void(double, double)>;

	/**
	 * Cursor within display callback function type.
	 * This function is called if the cursor leaves or enters the current display.
	 *
	 * @param bool: Cursor state.
	 */
	using TCursorWithinDisplayCallbackFunc = std::function<void(bool)>;

	/**
	 * Drag and drop callback function type.
	 *
	 * @param std::vector<std::filesystem::path>: The list of paths.
	 */
	using TDragAndDropCallbackFunc = std::function<void(std::vector<std::filesystem::path>)>;

	/**
	 * Display resize callback function type.
	 * This function is called once the display is resized.
	 *
	 * @param FExtent2D: The new display size.
	 */
	using TDisplayResizeCallbackFunc = std::function<void(FExtent2D)>;

	/**
	 * Display close callback function type.
	 * This function is called once the display is closed.
	 */
	using TDisplayCloseCallbackFunc = std::function<void()>;

	/**
	 * Flint display object.
	 * This object is responsible for a single display/ window instance.
	 */
	class FLINT_API Display : public FObject
	{
		friend Instance;

	public:
		Display(Instance& instance, const FExtent2D& extent, const std::string& title) : mInstance(instance), mExtent(extent), mTitle(title) {}

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
		virtual UI32 FindBestBufferCount(Device& device, UI32 count = 0) = 0;

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
		 * Set the key callback function.
		 *
		 * @param pFunction: The function pointer.
		 */
		void SetKeyCallback(const TKeyCallbackFunc& pFunction) { pKeyCallback = pFunction; }

		/**
		 * Set the mouse button callback function.
		 *
		 * @param pFunction: The function pointer.
		 */
		void SetMouseButtonCallback(const TMouseButtonCallbackFunc& pFunction) { pMouseButtonCallback = pFunction; }

		/**
		 * Set the cursor position callback function.
		 *
		 * @param pFunction: The function pointer.
		 */
		void SetCursorPositionCallback(const TCursorPositionCallbackFunc& pFunction) { pCursorPositionCallback = pFunction; }

		/**
		 * Set the mouse scroll callback function.
		 *
		 * @param pFunction: The function pointer.
		 */
		void SetMouseScrollCallback(const TMouseScrollCallbackFunc& pFunction) { pMouseScrollCallback = pFunction; }

		/**
		 * Set the cursor within display callback function.
		 *
		 * @param pFunction: The function pointer.
		 */
		void SetCursorWithinDisplayCallback(const TCursorWithinDisplayCallbackFunc& pFunction) { pCursorWithinDisplayCallback = pFunction; }

		/**
		 * Set the drag and drop callback function.
		 *
		 * @param pFunction: The function pointer.
		 */
		void SetDragAndDropCallback(const TDragAndDropCallbackFunc& pFunction) { pDragAndDropCallback = pFunction; }

		/**
		 * Set the display resize callback function.
		 *
		 * @param pFunction: The function pointer.
		 */
		void SetDisplayResizeCallback(const TDisplayResizeCallbackFunc& pFunction) { pDisplayResizeCallback = pFunction; }

		/**
		 * Set the display close callback function.
		 *
		 * @param pFunction: The function pointer.
		 */
		void SetDisplayCloseCallback(const TDisplayCloseCallbackFunc& pFunction) { pDisplayCloseCallback = pFunction; }

		/**
		 * Get the title of the display.
		 *
		 * @return The title string.
		 */
		std::string GetTitle() const { return mTitle; }

		/**
		 * Get the display's current extent.
		 *
		 * @return The extent.
		 */
		FExtent2D GetExtent() const { return mExtent; }

		/**
		 * Check if the display is open.
		 *
		 * @return Boolean value.
		 */
		bool IsOpen() const { return mIsDisplayOpen; }

		/**
		 * Check if the display is resized.
		 *
		 * @return Boolean value.
		 */
		bool IsDisplayResized() const { return mIsDislayResized; }

	protected:
		std::string mTitle = "";
		Instance& mInstance;
		FExtent2D mExtent = {};

		TKeyCallbackFunc pKeyCallback = nullptr;
		TMouseButtonCallbackFunc pMouseButtonCallback = nullptr;
		TCursorPositionCallbackFunc pCursorPositionCallback = nullptr;
		TMouseScrollCallbackFunc pMouseScrollCallback = nullptr;
		TCursorWithinDisplayCallbackFunc pCursorWithinDisplayCallback = nullptr;
		TDragAndDropCallbackFunc pDragAndDropCallback = nullptr;
		TDisplayResizeCallbackFunc pDisplayResizeCallback = nullptr;
		TDisplayCloseCallbackFunc pDisplayCloseCallback = nullptr;

		bool mIsDisplayOpen = true;
		bool mIsDislayResized = false;
	};
}