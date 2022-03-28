// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

#include <functional>

namespace Flint
{
	/**
	 * Event action enum.
	 */
	enum class EventAction : uint8_t
	{
		Released,
		Pressed,
		OnRepeat
	};

	/**
	 * Key code enum.
	 */
	enum class KeyCode : uint8_t
	{
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
	enum class SpecialCharacter : uint8_t
	{
		Shift = BitShiftLeft(0),
		Control = BitShiftLeft(1),
		Alt = BitShiftLeft(2),
		Super = BitShiftLeft(3),
		CapsLock = BitShiftLeft(4),
		NumLock = BitShiftLeft(5)
	};

	constexpr SpecialCharacter operator|(const SpecialCharacter& lhs, const SpecialCharacter& rhs) { return SpecialCharacter(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)); }
	constexpr SpecialCharacter operator&(const SpecialCharacter& lhs, const SpecialCharacter& rhs) { return SpecialCharacter(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)); }

	/**
	 * Mouse button enum.
	 */
	enum class MouseButton : uint8_t
	{
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
		 * @param action The action of the event.
		 * @param special The special character of the event.
		 */
		ButtonEvent(EventAction action, SpecialCharacter special) : m_Action(action), m_SpecialCharacter(special) {}

		/**
		 * Activate the event.
		 *
		 * @param action The action of the event.
		 * @param special The special character of th event.
		 */
		void Activate(EventAction action, SpecialCharacter special) { m_Action = action, m_SpecialCharacter = special; }

		/**
		 * Reset the event back to its defaults.
		 */
		void Reset() { m_Action = EventAction::Released, m_SpecialCharacter = SpecialCharacter(0); }

		/**
		 * Get the event action.
		 *
		 * @return The event action.
		 */
		const EventAction GetEventAction() const { return m_Action; }

		/**
		 * Get the special character of the event.
		 *
		 * @return The special character.
		 */
		const SpecialCharacter GetSpecialCharacter() const { return m_SpecialCharacter; }

		/**
		 * Check if the event is released.
		 *
		 * @return Boolean value.
		 */
		const bool IsReleased() const { return m_Action == EventAction::Released; }

		/**
		 * Check if the event is pressed.
		 *
		 * @return Boolean value.
		 */
		const bool IsPressed() const { return m_Action == EventAction::Pressed; }

		/**
		 * Check if the event is on repeat.
		 *
		 * @return Boolean value.
		 */
		const bool IsOnRepeat() const { return m_Action == EventAction::OnRepeat; }

		EventAction m_Action = EventAction::Released;
		SpecialCharacter m_SpecialCharacter = SpecialCharacter(0);
	};

	/**
	 * Flint display object.
	 * This object is responsible for a single display/ window instance.
	 */
	template<class InstanceT, class DeviceT>
	class Display : public FObject
	{
		friend InstanceT;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pInstance The instance pointer.
		 * @param extent The display extent. If set to (0, 0), the window will be opened in full screen mode. If supplied with an exponentially large number, the window will open in
		 *				  maximized mode.
		 * @param title The display title.
		 */
		Display(InstanceT* pInstance, const FBox2D& extent, const std::string& title) 
			: pInstance(pInstance), m_Extent(extent), m_Title(title)
		{
			if (!pInstance)
				throw std::invalid_argument("Instance pointer should not be null!");
		}

		/**
		 * Update the display object.
		 * This polls inputs and prepares the display for the next draw.
		 */
		virtual void Update() = 0;

		/**
		 * Find the best buffer count for the swap chain.
		 * If 0 is passed (by default), the minimum supported buffer count is given. If UI32_MAX is give, the maximum supported is given.
		 *
		 * @param device The device object to check with.
		 * @param count The count that needs to be checked. Default is 0.
		 * @return The best buffer count integer.
		 */
		virtual uint32_t FindBestBufferCount(DeviceT* pDevice, const uint32_t count = 0) = 0;

		/**
		 * Get the best swap chain format.
		 *
		 * @param pDevice The device pointer.
		 * @return The swap chain image format.
		 */
		virtual PixelFormat GetBestSwapChainFormat(DeviceT* pDevice) = 0;

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
		const bool ContainsDragAndDrop() const { return !m_DragAndDropPaths.empty(); }

		/**
		 * Get the drag and drop values.
		 * Once you get this value, it will automatically clear itself from the display.
		 *
		 * @return The vector of paths.
		 */
		const std::vector<std::filesystem::path> GetDragAndDropValues()
		{
			return std::move(m_DragAndDropPaths);
		}

		/**
		 * Get the title of the display.
		 *
		 * @return The title string.
		 */
		const std::string GetTitle() const { return m_Title; }

		/**
		 * Set a new window title.
		 *
		 * @param title The window title to set.
		 */
		virtual void SetTitle(const std::string& title) = 0;

		/**
		 * Set a new extent to the window.
		 *
		 * @param newExtent The new extent to set.
		 */
		virtual void SetExtent(FBox2D newExtent) = 0;

		/**
		 * Get the display's current extent.
		 *
		 * @return The extent.
		 */
		const FBox2D GetExtent() const { return m_Extent; }

		/**
		 * Get a key event from the event store.
		 *
		 * @param key The key code.
		 * @return The button event.
		 */
		const ButtonEvent GetKeyEvent(KeyCode key) const { return m_KeyEvents[static_cast<uint8_t>(key)]; }

		/**
		 * Get a mouse button event from the event store.
		 *
		 * @param button The mouse button code.
		 * @return The button event.
		 */
		const ButtonEvent GetMouseButtonEvent(MouseButton button) const { return m_MouseButtonEvents[static_cast<uint8_t>(button)]; }

		/**
		 * Get the mouse position.
		 *
		 * @return The position.
		 */
		const FExtent2D<float> GetMousePosition() const { return FExtent2D<float>(m_MousePositionX, m_MousePositionY); }

		/**
		 * Get the mouse scroll.
		 *
		 * @return The mouse scroll as (up, down).
		 */
		const FExtent2D<float> GetMouseScroll() const { return FExtent2D<float>(m_MouseScrollUp, m_MouseScrollDown); }

		/**
		 * Check if the display is open.
		 *
		 * @return Boolean value.
		 */
		const bool IsOpen() const { return m_IsDisplayOpen; }

		/**
		 * Check if the display is resized.
		 *
		 * @return Boolean value.
		 */
		const bool IsDisplayResized() const { return m_IsDislayResized; }

		/**
		 * Check if the cursor is within the display.
		 *
		 * @return Boolean value.
		 */
		const bool IsCursorWithinDisplay() const { return m_IsCursorWithinDisplay; }

	protected:
		std::vector<std::filesystem::path> m_DragAndDropPaths;

		std::string m_Title = "";
		InstanceT* pInstance = nullptr;
		FBox2D m_Extent = {};

		ButtonEvent m_KeyEvents[static_cast<uint8_t>(KeyCode::KeyCodeMax)] = {};
		ButtonEvent m_MouseButtonEvents[static_cast<uint8_t>(MouseButton::MouseButtonMax)] = {};

		float m_MousePositionX = 0.0f;
		float m_MousePositionY = 0.0f;

		float m_MouseScrollUp = 0.0f;
		float m_MouseScrollDown = 0.0f;

		bool m_IsDisplayOpen = true;
		bool m_IsDislayResized = false;
		bool m_IsCursorWithinDisplay = false;
	};
}