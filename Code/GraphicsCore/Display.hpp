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
		Released,
		Pressed,
		OnRepeat
	};

	/**
	 * Key code enum.
	 */
	enum class KeyCode : UI8 {
		Unknown,

		Space,
		Apostrophe,

		Comma, Minus, Preiod, Slash,

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
		PageDowm,
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

		KeyCodeMax
	};

	/**
	 * Special characters enum.
	 */
	enum class SpecialCharacter : UI8 {
		Shift = BitShiftLeft(0),
		Control = BitShiftLeft(1),
		Alt = BitShiftLeft(2),
		Super = BitShiftLeft(3),
		CapsLock = BitShiftLeft(4),
		NumLock = BitShiftLeft(5)
	};

	constexpr SpecialCharacter operator|(const SpecialCharacter& lhs, const SpecialCharacter& rhs) { return SpecialCharacter(static_cast<UI8>(lhs) | static_cast<UI8>(rhs)); }
	constexpr SpecialCharacter operator&(const SpecialCharacter& lhs, const SpecialCharacter& rhs) { return SpecialCharacter(static_cast<UI8>(lhs) & static_cast<UI8>(rhs)); }

	/**
	 * Mouse button enum.
	 */
	enum class MouseButton : UI8 {
		ButtonOne, ButtonTwo, ButtonThree, ButtonFour,
		ButtonFive, ButtonSix, ButtonSeven, ButtonEight,

		Last, Left,
		Right, Middle,

		MouseButtonMax
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
		void Reset() { mAction = EventAction::Released, mSpecialCharacter = SpecialCharacter(0); }

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
		const bool IsReleased() const { return mAction == EventAction::Released; }

		/**
		 * Check if the event is pressed.
		 *
		 * @return Boolean value.
		 */
		const bool IsPressed() const { return mAction == EventAction::Pressed; }

		/**
		 * Check if the event is on repeat.
		 *
		 * @return Boolean value.
		 */
		const bool IsOnRepeat() const { return mAction == EventAction::OnRepeat; }

		EventAction mAction = EventAction::Released;
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
		 * @param extent: The display extent. If set to (0, 0), the window will be opened in full screen mode. If supplied with an exponentially large number, the window will open in
		 *				  maximized mode.
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
		virtual UI32 FindBestBufferCount(const std::shared_ptr<Device>& pDevice, const UI32 count = 0) = 0;

		/**
		 * Get the best swap chain format.
		 * 
		 * @param pDevice: The device pointer.
		 * @return The swap chain image format.
		 */
		virtual PixelFormat GetBestSwapChainFormat(const std::shared_ptr<Device>& pDevice) = 0;

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
		const bool ContainsDragAndDrop() const { return !mDragAndDropPaths.empty(); }

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

		ButtonEvent mKeyEvents[static_cast<UI8>(KeyCode::KeyCodeMax)] = {};
		ButtonEvent mMouseButtonEvents[static_cast<UI8>(MouseButton::MouseButtonMax)] = {};

		float mMousePositionX = 0.0f;
		float mMousePositionY = 0.0f;

		float mMouseScrollUp = 0.0f;
		float mMouseScrollDown = 0.0f;

		bool mIsDisplayOpen = true;
		bool mIsDislayResized = false;
		bool mIsCursorWithinDisplay = false;
	};
}