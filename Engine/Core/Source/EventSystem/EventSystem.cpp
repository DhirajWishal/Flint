// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/EventSystem/EventSystem.hpp"
#include "Core/Window.hpp"

#include <SDL.h>

namespace Flint
{
	EventType EventSystem::poll()
	{
		SDL_Event sdlEvent = {};
		if (SDL_PollEvent(&sdlEvent))
		{
			// Handle the events.
			switch (sdlEvent.type)
			{
			case SDL_FIRSTEVENT:
				break;

			case SDL_QUIT:
			case SDL_APP_TERMINATING:
				m_ShouldClose = true;
				return EventType::Close;

			case SDL_APP_LOWMEMORY:
			case SDL_APP_WILLENTERBACKGROUND:
			case SDL_APP_DIDENTERBACKGROUND:
			case SDL_APP_WILLENTERFOREGROUND:
			case SDL_APP_DIDENTERFOREGROUND:
				break;

			case SDL_LOCALECHANGED:
				break;

			case SDL_DISPLAYEVENT:
				break;

			case SDL_WINDOWEVENT:
				//const auto pWindow = reinterpret_cast<Window*>(SDL_GetWindowData(SDL_GetWindowFromID(sdlEvent.window.windowID), "this"));
				break;

			case SDL_SYSWMEVENT:
				break;

			case SDL_KEYDOWN:
				handleKeyEvent(sdlEvent.key.keysym.scancode, true);
				handleSpecialCharacter(sdlEvent.key.keysym.mod, true);
				return EventType::Keyboard;

			case SDL_KEYUP:
				handleKeyEvent(sdlEvent.key.keysym.scancode, false);
				handleSpecialCharacter(sdlEvent.key.keysym.mod, false);
				return EventType::Keyboard;

			case SDL_TEXTEDITING:
				break;

			case SDL_TEXTINPUT:
				m_Keyboard.m_TextInput = sdlEvent.text.text;
				return EventType::Keyboard;

			case SDL_KEYMAPCHANGED:
				break;

			case SDL_TEXTEDITING_EXT:
				break;

			case SDL_MOUSEMOTION:
				m_Mouse.m_PositionX = static_cast<float>(sdlEvent.motion.x);
				m_Mouse.m_PositionY = static_cast<float>(sdlEvent.motion.y);
				return EventType::Mouse;

			case SDL_MOUSEBUTTONDOWN:
				handleMouseButtonEvent(sdlEvent.button.button, sdlEvent.button.clicks);
				return EventType::Mouse;

			case SDL_MOUSEBUTTONUP:
				handleMouseButtonEvent(sdlEvent.button.button, 0);
				return EventType::Mouse;

			case SDL_MOUSEWHEEL:
				m_Mouse.m_ScrollX = sdlEvent.wheel.preciseX;
				m_Mouse.m_ScrollY = sdlEvent.wheel.preciseY;
				return EventType::Mouse;

			case SDL_JOYAXISMOTION:
			case SDL_JOYBALLMOTION:
			case SDL_JOYHATMOTION:
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			case SDL_JOYDEVICEADDED:
			case SDL_JOYDEVICEREMOVED:
			case SDL_JOYBATTERYUPDATED:
				break;

			case SDL_CONTROLLERAXISMOTION:
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
			case SDL_CONTROLLERDEVICEREMAPPED:
			case SDL_CONTROLLERTOUCHPADDOWN:
			case SDL_CONTROLLERTOUCHPADMOTION:
			case SDL_CONTROLLERTOUCHPADUP:
			case SDL_CONTROLLERSENSORUPDATE:
				break;

			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			case SDL_FINGERMOTION:
				break;

			case SDL_DOLLARGESTURE:
			case SDL_DOLLARRECORD:
				break;

			case SDL_MULTIGESTURE:
				break;

			case SDL_CLIPBOARDUPDATE:
				break;

			case SDL_DROPFILE:
				break;

			case SDL_DROPTEXT:
				break;

			case SDL_DROPBEGIN:
				break;

			case SDL_DROPCOMPLETE:
				break;

			case SDL_AUDIODEVICEADDED:
			case SDL_AUDIODEVICEREMOVED:
				break;

			case SDL_SENSORUPDATE:
				break;
			case SDL_RENDER_TARGETS_RESET:
				break;
			case SDL_RENDER_DEVICE_RESET:
				break;

			case SDL_POLLSENTINEL:
				break;

			case SDL_USEREVENT:
				break;

			case SDL_LASTEVENT:
				break;

			default:
				break;
			}
		}

		return EventType::None;
	}

	void EventSystem::handleKeyEvent(int32_t scancode, bool pressed)
	{
		switch (scancode)
		{
		case SDL_SCANCODE_RETURN:			m_Keyboard.m_Enter = pressed; break;
		case SDL_SCANCODE_ESCAPE:			m_Keyboard.m_Escape = pressed; break;
		case SDL_SCANCODE_BACKSPACE:		m_Keyboard.m_Backspace = pressed; break;
		case SDL_SCANCODE_TAB:				m_Keyboard.m_Tab = pressed; break;
		case SDL_SCANCODE_SPACE:			m_Keyboard.m_Space = pressed; break;

		case SDL_SCANCODE_A:				m_Keyboard.m_KeyA = pressed; break;
		case SDL_SCANCODE_B:				m_Keyboard.m_KeyB = pressed; break;
		case SDL_SCANCODE_C:				m_Keyboard.m_KeyC = pressed; break;
		case SDL_SCANCODE_D:				m_Keyboard.m_KeyD = pressed; break;
		case SDL_SCANCODE_E:				m_Keyboard.m_KeyE = pressed; break;
		case SDL_SCANCODE_F:				m_Keyboard.m_KeyF = pressed; break;
		case SDL_SCANCODE_G:				m_Keyboard.m_KeyG = pressed; break;
		case SDL_SCANCODE_H:				m_Keyboard.m_KeyH = pressed; break;
		case SDL_SCANCODE_I:				m_Keyboard.m_KeyI = pressed; break;
		case SDL_SCANCODE_J:				m_Keyboard.m_KeyJ = pressed; break;
		case SDL_SCANCODE_K:				m_Keyboard.m_KeyK = pressed; break;
		case SDL_SCANCODE_L:				m_Keyboard.m_KeyL = pressed; break;
		case SDL_SCANCODE_M:				m_Keyboard.m_KeyM = pressed; break;
		case SDL_SCANCODE_N:				m_Keyboard.m_KeyN = pressed; break;
		case SDL_SCANCODE_O:				m_Keyboard.m_KeyO = pressed; break;
		case SDL_SCANCODE_P:				m_Keyboard.m_KeyP = pressed; break;
		case SDL_SCANCODE_Q:				m_Keyboard.m_KeyQ = pressed; break;
		case SDL_SCANCODE_R:				m_Keyboard.m_KeyR = pressed; break;
		case SDL_SCANCODE_S:				m_Keyboard.m_KeyS = pressed; break;
		case SDL_SCANCODE_T:				m_Keyboard.m_KeyT = pressed; break;
		case SDL_SCANCODE_U:				m_Keyboard.m_KeyU = pressed; break;
		case SDL_SCANCODE_V:				m_Keyboard.m_KeyV = pressed; break;
		case SDL_SCANCODE_W:				m_Keyboard.m_KeyW = pressed; break;
		case SDL_SCANCODE_X:				m_Keyboard.m_KeyX = pressed; break;
		case SDL_SCANCODE_Y:				m_Keyboard.m_KeyY = pressed; break;
		case SDL_SCANCODE_Z:				m_Keyboard.m_KeyZ = pressed; break;

		case SDL_SCANCODE_1:				m_Keyboard.m_KeyOne = pressed; break;
		case SDL_SCANCODE_2:				m_Keyboard.m_KeyTwo = pressed; break;
		case SDL_SCANCODE_3:				m_Keyboard.m_KeyThree = pressed; break;
		case SDL_SCANCODE_4:				m_Keyboard.m_KeyFour = pressed; break;
		case SDL_SCANCODE_5:				m_Keyboard.m_KeyFive = pressed; break;
		case SDL_SCANCODE_6:				m_Keyboard.m_KeySix = pressed; break;
		case SDL_SCANCODE_7:				m_Keyboard.m_KeySeven = pressed; break;
		case SDL_SCANCODE_8:				m_Keyboard.m_KeyEight = pressed; break;
		case SDL_SCANCODE_9:				m_Keyboard.m_KeyNine = pressed; break;
		case SDL_SCANCODE_0:				m_Keyboard.m_KeyZero = pressed; break;

		case SDL_SCANCODE_MINUS:			m_Keyboard.m_Minus = pressed; break;
		case SDL_SCANCODE_EQUALS:			m_Keyboard.m_Equal = pressed; break;
		case SDL_SCANCODE_LEFTBRACKET:		m_Keyboard.m_LeftBracket = pressed; break;
		case SDL_SCANCODE_RIGHTBRACKET:		m_Keyboard.m_RightBracket = pressed; break;
		case SDL_SCANCODE_BACKSLASH:		m_Keyboard.m_Backslash = pressed; break;

		case SDL_SCANCODE_SEMICOLON:		m_Keyboard.m_Semicolon = pressed; break;
		case SDL_SCANCODE_APOSTROPHE:		m_Keyboard.m_Apostrophe = pressed; break;
		case SDL_SCANCODE_GRAVE:			m_Keyboard.m_GraveAccent = pressed; break;

		case SDL_SCANCODE_COMMA:			m_Keyboard.m_Comma = pressed; break;
		case SDL_SCANCODE_PERIOD:			m_Keyboard.m_Period = pressed; break;
		case SDL_SCANCODE_SLASH:			m_Keyboard.m_Slash = pressed; break;

		case SDL_SCANCODE_CAPSLOCK:			m_Keyboard.m_CapsLock = pressed; break;

		case SDL_SCANCODE_F1:				m_Keyboard.m_F1 = pressed; break;
		case SDL_SCANCODE_F2:				m_Keyboard.m_F2 = pressed; break;
		case SDL_SCANCODE_F3:				m_Keyboard.m_F3 = pressed; break;
		case SDL_SCANCODE_F4:				m_Keyboard.m_F4 = pressed; break;
		case SDL_SCANCODE_F5:				m_Keyboard.m_F5 = pressed; break;
		case SDL_SCANCODE_F6:				m_Keyboard.m_F6 = pressed; break;
		case SDL_SCANCODE_F7:				m_Keyboard.m_F7 = pressed; break;
		case SDL_SCANCODE_F8:				m_Keyboard.m_F8 = pressed; break;
		case SDL_SCANCODE_F9:				m_Keyboard.m_F9 = pressed; break;
		case SDL_SCANCODE_F10:				m_Keyboard.m_F10 = pressed; break;
		case SDL_SCANCODE_F11:				m_Keyboard.m_F11 = pressed; break;
		case SDL_SCANCODE_F12:				m_Keyboard.m_F12 = pressed; break;

		case SDL_SCANCODE_PRINTSCREEN:		m_Keyboard.m_PrintScreen = pressed; break;
		case SDL_SCANCODE_SCROLLLOCK:		m_Keyboard.m_ScrollLock = pressed; break;
		case SDL_SCANCODE_PAUSE:			m_Keyboard.m_Pause = pressed; break;
		case SDL_SCANCODE_INSERT:			m_Keyboard.m_Insert = pressed; break;

		case SDL_SCANCODE_HOME:				m_Keyboard.m_Home = pressed; break;
		case SDL_SCANCODE_PAGEUP:			m_Keyboard.m_PageUp = pressed; break;
		case SDL_SCANCODE_DELETE:			m_Keyboard.m_PageDown = pressed; break;
		case SDL_SCANCODE_END:				m_Keyboard.m_End = pressed; break;
		case SDL_SCANCODE_PAGEDOWN:			m_Keyboard.m_PageDown = pressed; break;
		case SDL_SCANCODE_RIGHT:			m_Keyboard.m_Right = pressed; break;
		case SDL_SCANCODE_LEFT:				m_Keyboard.m_Left = pressed; break;
		case SDL_SCANCODE_DOWN:				m_Keyboard.m_Down = pressed; break;
		case SDL_SCANCODE_UP:				m_Keyboard.m_Up = pressed; break;

		case SDL_SCANCODE_NUMLOCKCLEAR:		m_Keyboard.m_NumLock = pressed; break;

		case SDL_SCANCODE_KP_DIVIDE:		m_Keyboard.m_KeyPadDivide = pressed; break;
		case SDL_SCANCODE_KP_MULTIPLY:		m_Keyboard.m_KeyPadMultiply = pressed; break;
		case SDL_SCANCODE_KP_MINUS:			m_Keyboard.m_KeyPadSubtract = pressed; break;
		case SDL_SCANCODE_KP_PLUS:			m_Keyboard.m_KeyPadAdd = pressed; break;
		case SDL_SCANCODE_KP_ENTER:			m_Keyboard.m_KeyPadEnter = pressed; break;
		case SDL_SCANCODE_KP_1:				m_Keyboard.m_KeyPadOne = pressed; break;
		case SDL_SCANCODE_KP_2:				m_Keyboard.m_KeyPadTwo = pressed; break;
		case SDL_SCANCODE_KP_3:				m_Keyboard.m_KeyPadThree = pressed; break;
		case SDL_SCANCODE_KP_4:				m_Keyboard.m_KeyPadFour = pressed; break;
		case SDL_SCANCODE_KP_5:				m_Keyboard.m_KeyPadFive = pressed; break;
		case SDL_SCANCODE_KP_6:				m_Keyboard.m_KeyPadSix = pressed; break;
		case SDL_SCANCODE_KP_7:				m_Keyboard.m_KeyPadSeven = pressed; break;
		case SDL_SCANCODE_KP_8:				m_Keyboard.m_KeyPadEight = pressed; break;
		case SDL_SCANCODE_KP_9:				m_Keyboard.m_KeyPadNine = pressed; break;
		case SDL_SCANCODE_KP_0:				m_Keyboard.m_KeyPadZero = pressed; break;
		case SDL_SCANCODE_KP_PERIOD:		m_Keyboard.m_KeyPadDecimal = pressed; break;

		default:							break;
		}
	}

	void EventSystem::handleSpecialCharacter(uint64_t characters, bool pressed)
	{
		if (characters & KMOD_LCTRL) m_Keyboard.m_LeftControl = pressed;
		if (characters & KMOD_RCTRL) m_Keyboard.m_RightControl = pressed;
		if (characters & KMOD_LSHIFT) m_Keyboard.m_LeftShift = pressed;
		if (characters & KMOD_RSHIFT) m_Keyboard.m_RightShift = pressed;
		if (characters & KMOD_LALT) m_Keyboard.m_LeftAlt = pressed;
		if (characters & KMOD_RALT) m_Keyboard.m_RightAlt = pressed;
		if (characters & KMOD_LGUI) m_Keyboard.m_LeftSuper = pressed;
		if (characters & KMOD_RGUI) m_Keyboard.m_RightSuper = pressed;
		if (characters & KMOD_NUM) m_Keyboard.m_NumLock = pressed;
		if (characters & KMOD_CAPS) m_Keyboard.m_CapsLock = pressed;
		if (characters & KMOD_SCROLL) m_Keyboard.m_ScrollLock = pressed;
	}

	void EventSystem::handleMouseButtonEvent(uint8_t button, uint8_t clicks)
	{
		if (button == SDL_BUTTON_LEFT) m_Mouse.m_Left = clicks;
		else if (button == SDL_BUTTON_RIGHT) m_Mouse.m_Right = clicks;
		else if (button == SDL_BUTTON_MIDDLE) m_Mouse.m_Middle = clicks;
		else if (button == SDL_BUTTON_X1) m_Mouse.m_Aux1 = clicks;
		else if (button == SDL_BUTTON_X2) m_Mouse.m_Aux2 = clicks;
	}
}