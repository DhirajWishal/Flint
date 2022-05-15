// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Window.hpp"

#include <SDL.h>

namespace /* anonymous */
{
	/**
	 * Static initializer struct.
	 * This struct is used to initialize and destroy the SDL library.
	 */
	struct StaticInitializer
	{
		/**
		 * Default constructor.
		 */
		StaticInitializer()
		{
			// Initialize SDL.
			SDL_Init(SDL_INIT_VIDEO);
		}

		/**
		 * Destructor.
		 */
		~StaticInitializer()
		{
			// Quit SDL.
			SDL_Quit();
		}
	};
}

namespace Flint
{
	Window::Window()
	{
		// Create the static initializer.
		static StaticInitializer initializer;
	}

	bool Window::pollEvents()
	{
		SDL_Event sdlEvent = {};
		if (SDL_PollEvent(&sdlEvent))
		{
			// Handle the events.
			switch (sdlEvent.type)
			{
				// Some of the ones that we don't need to do anything with. They're just there for the future.
			case SDL_FIRSTEVENT:
				break;

			case SDL_QUIT:
				return false;

			case SDL_APP_TERMINATING:
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
				break;

			case SDL_SYSWMEVENT:
				break;

			case SDL_KEYDOWN:
				handleKeyEvent(sdlEvent.key.keysym.scancode, true);
				handleSpecialCharacter(sdlEvent.key.keysym.mod, true);
				break;

			case SDL_KEYUP:
				handleKeyEvent(sdlEvent.key.keysym.scancode, false);
				handleSpecialCharacter(sdlEvent.key.keysym.mod, false);
				break;

			case SDL_TEXTEDITING:
				break;

			case SDL_TEXTINPUT:
				break;

			case SDL_KEYMAPCHANGED:
				break;

			case SDL_TEXTEDITING_EXT:
				break;

			case SDL_MOUSEMOTION:
				break;

			case SDL_MOUSEBUTTONDOWN:
				handleMouseButtonEvent(sdlEvent.button.button, true);
				break;

			case SDL_MOUSEBUTTONUP:
				handleMouseButtonEvent(sdlEvent.button.button, false);
				break;

			case SDL_MOUSEWHEEL:
				break;

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

		return true;
	}

	void Window::handleKeyEvent(int32_t scancode, bool pressed)
	{
		KeyCode code = KeyCode::Unknown;

		switch (scancode)
		{
		case SDL_SCANCODE_RETURN:			code = KeyCode::Enter; break;
		case SDL_SCANCODE_ESCAPE:			code = KeyCode::Escape; break;
		case SDL_SCANCODE_BACKSPACE:		code = KeyCode::Backspace; break;
		case SDL_SCANCODE_TAB:				code = KeyCode::Tab; break;
		case SDL_SCANCODE_SPACE:			code = KeyCode::Space; break;

		case SDL_SCANCODE_A:				code = KeyCode::KeyA; break;
		case SDL_SCANCODE_B:				code = KeyCode::KeyB; break;
		case SDL_SCANCODE_C:				code = KeyCode::KeyC; break;
		case SDL_SCANCODE_D:				code = KeyCode::KeyD; break;
		case SDL_SCANCODE_E:				code = KeyCode::KeyE; break;
		case SDL_SCANCODE_F:				code = KeyCode::KeyF; break;
		case SDL_SCANCODE_G:				code = KeyCode::KeyG; break;
		case SDL_SCANCODE_H:				code = KeyCode::KeyH; break;
		case SDL_SCANCODE_I:				code = KeyCode::KeyI; break;
		case SDL_SCANCODE_J:				code = KeyCode::KeyJ; break;
		case SDL_SCANCODE_K:				code = KeyCode::KeyK; break;
		case SDL_SCANCODE_L:				code = KeyCode::KeyL; break;
		case SDL_SCANCODE_M:				code = KeyCode::KeyM; break;
		case SDL_SCANCODE_N:				code = KeyCode::KeyN; break;
		case SDL_SCANCODE_O:				code = KeyCode::KeyO; break;
		case SDL_SCANCODE_P:				code = KeyCode::KeyP; break;
		case SDL_SCANCODE_Q:				code = KeyCode::KeyQ; break;
		case SDL_SCANCODE_R:				code = KeyCode::KeyR; break;
		case SDL_SCANCODE_S:				code = KeyCode::KeyS; break;
		case SDL_SCANCODE_T:				code = KeyCode::KeyT; break;
		case SDL_SCANCODE_U:				code = KeyCode::KeyU; break;
		case SDL_SCANCODE_V:				code = KeyCode::KeyV; break;
		case SDL_SCANCODE_W:				code = KeyCode::KeyW; break;
		case SDL_SCANCODE_X:				code = KeyCode::KeyX; break;
		case SDL_SCANCODE_Y:				code = KeyCode::KeyY; break;
		case SDL_SCANCODE_Z:				code = KeyCode::KeyZ; break;

		case SDL_SCANCODE_1:				code = KeyCode::KeyOne; break;
		case SDL_SCANCODE_2:				code = KeyCode::KeyTwo; break;
		case SDL_SCANCODE_3:				code = KeyCode::KeyThree; break;
		case SDL_SCANCODE_4:				code = KeyCode::KeyFour; break;
		case SDL_SCANCODE_5:				code = KeyCode::KeyFive; break;
		case SDL_SCANCODE_6:				code = KeyCode::KeySix; break;
		case SDL_SCANCODE_7:				code = KeyCode::KeySeven; break;
		case SDL_SCANCODE_8:				code = KeyCode::KeyEight; break;
		case SDL_SCANCODE_9:				code = KeyCode::KeyNine; break;
		case SDL_SCANCODE_0:				code = KeyCode::KeyZero; break;

		case SDL_SCANCODE_MINUS:			code = KeyCode::Minus; break;
		case SDL_SCANCODE_EQUALS:			code = KeyCode::Equal; break;
		case SDL_SCANCODE_LEFTBRACKET:		code = KeyCode::LeftBracket; break;
		case SDL_SCANCODE_RIGHTBRACKET:		code = KeyCode::RightBracket; break;
		case SDL_SCANCODE_BACKSLASH:		code = KeyCode::Backslash; break;

		case SDL_SCANCODE_SEMICOLON:		code = KeyCode::Semicolon; break;
		case SDL_SCANCODE_APOSTROPHE:		code = KeyCode::Apostrophe; break;
		case SDL_SCANCODE_GRAVE:			code = KeyCode::GraveAccent; break;

		case SDL_SCANCODE_COMMA:			code = KeyCode::Comma; break;
		case SDL_SCANCODE_PERIOD:			code = KeyCode::Period; break;
		case SDL_SCANCODE_SLASH:			code = KeyCode::Slash; break;

		case SDL_SCANCODE_CAPSLOCK:			code = KeyCode::CapsLock; break;

		case SDL_SCANCODE_F1:				code = KeyCode::F1; break;
		case SDL_SCANCODE_F2:				code = KeyCode::F2; break;
		case SDL_SCANCODE_F3:				code = KeyCode::F3; break;
		case SDL_SCANCODE_F4:				code = KeyCode::F4; break;
		case SDL_SCANCODE_F5:				code = KeyCode::F5; break;
		case SDL_SCANCODE_F6:				code = KeyCode::F6; break;
		case SDL_SCANCODE_F7:				code = KeyCode::F7; break;
		case SDL_SCANCODE_F8:				code = KeyCode::F8; break;
		case SDL_SCANCODE_F9:				code = KeyCode::F9; break;
		case SDL_SCANCODE_F10:				code = KeyCode::F10; break;
		case SDL_SCANCODE_F11:				code = KeyCode::F11; break;
		case SDL_SCANCODE_F12:				code = KeyCode::F12; break;

		case SDL_SCANCODE_PRINTSCREEN:		code = KeyCode::PrintScreen; break;
		case SDL_SCANCODE_SCROLLLOCK:		code = KeyCode::ScrollLock; break;
		case SDL_SCANCODE_PAUSE:			code = KeyCode::Pause; break;
		case SDL_SCANCODE_INSERT:			code = KeyCode::Insert; break;

		case SDL_SCANCODE_HOME:				code = KeyCode::Home; break;
		case SDL_SCANCODE_PAGEUP:			code = KeyCode::PageUp; break;
		case SDL_SCANCODE_DELETE:			code = KeyCode::PageDown; break;
		case SDL_SCANCODE_END:				code = KeyCode::End; break;
		case SDL_SCANCODE_PAGEDOWN:			code = KeyCode::PageDown; break;
		case SDL_SCANCODE_RIGHT:			code = KeyCode::Right; break;
		case SDL_SCANCODE_LEFT:				code = KeyCode::Left; break;
		case SDL_SCANCODE_DOWN:				code = KeyCode::Down; break;
		case SDL_SCANCODE_UP:				code = KeyCode::Up; break;

		case SDL_SCANCODE_NUMLOCKCLEAR:		code = KeyCode::NumLock; break;

		case SDL_SCANCODE_KP_DIVIDE:		code = KeyCode::KeyPadDivide; break;
		case SDL_SCANCODE_KP_MULTIPLY:		code = KeyCode::KeyPadMultiply; break;
		case SDL_SCANCODE_KP_MINUS:			code = KeyCode::KeyPadSubtract; break;
		case SDL_SCANCODE_KP_PLUS:			code = KeyCode::KeyPadAdd; break;
		case SDL_SCANCODE_KP_ENTER:			code = KeyCode::KeyPadEnter; break;
		case SDL_SCANCODE_KP_1:				code = KeyCode::KeyPadOne; break;
		case SDL_SCANCODE_KP_2:				code = KeyCode::KeyPadTwo; break;
		case SDL_SCANCODE_KP_3:				code = KeyCode::KeyPadThree; break;
		case SDL_SCANCODE_KP_4:				code = KeyCode::KeyPadFour; break;
		case SDL_SCANCODE_KP_5:				code = KeyCode::KeyPadFive; break;
		case SDL_SCANCODE_KP_6:				code = KeyCode::KeyPadSix; break;
		case SDL_SCANCODE_KP_7:				code = KeyCode::KeyPadSeven; break;
		case SDL_SCANCODE_KP_8:				code = KeyCode::KeyPadEight; break;
		case SDL_SCANCODE_KP_9:				code = KeyCode::KeyPadNine; break;
		case SDL_SCANCODE_KP_0:				code = KeyCode::KeyPadZero; break;
		case SDL_SCANCODE_KP_PERIOD:		code = KeyCode::KeyPadDecimal; break;

		default:							break;
		}

		if (pressed)
			m_EventSummary.pressed(code);
		else
			m_EventSummary.released(code);
	}

	void Window::handleSpecialCharacter(uint64_t characters, bool pressed)
	{
		uint32_t character = 0;
		if (characters & KMOD_LCTRL)
			character |= static_cast<uint32_t>(SpecialCharacter::LeftControl);

		if (characters & KMOD_RCTRL)
			character |= static_cast<uint32_t>(SpecialCharacter::RightControl);

		if (characters & KMOD_LSHIFT)
			character |= static_cast<uint32_t>(SpecialCharacter::LeftShift);

		if (characters & KMOD_RSHIFT)
			character |= static_cast<uint32_t>(SpecialCharacter::RightShift);

		if (characters & KMOD_LALT)
			character |= static_cast<uint32_t>(SpecialCharacter::LeftAlt);

		if (characters & KMOD_RALT)
			character |= static_cast<uint32_t>(SpecialCharacter::RightAlt);

		if (characters & KMOD_GUI)
			character |= static_cast<uint32_t>(SpecialCharacter::Super);

		if (pressed)
			m_EventSummary.pressed(static_cast<SpecialCharacter>(character));
		else
			m_EventSummary.released(static_cast<SpecialCharacter>(character));
	}

	void Window::handleMouseButtonEvent(uint8_t button, bool pressed)
	{
		// Left mouse button press.
		if (button == SDL_BUTTON_LEFT)
		{
			if (pressed)
				m_EventSummary.pressed(MouseButton::Left);
			else
				m_EventSummary.released(MouseButton::Left);
		}


		// Right mouse button press.
		else if (button == SDL_BUTTON_RIGHT)
		{
			if (pressed)
				m_EventSummary.pressed(MouseButton::Right);
			else
				m_EventSummary.released(MouseButton::Right);
		}

		// Middle mouse button press.
		else if (button == SDL_BUTTON_MIDDLE)
		{
			if (pressed)
				m_EventSummary.pressed(MouseButton::Middle);
			else
				m_EventSummary.released(MouseButton::Middle);
		}
	}
}