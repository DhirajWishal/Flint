// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine.hpp"

namespace Flint
{
	/**
	 * Window class.
	 * This is a special class, as it contains all the processing stages and finally renders the image to the user using the window created as specified.
	 */
	class Window : public FObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine to which this window is bound to.
		 * @param title The window title.
		 * @param width The width of the window. If set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
		 * @param height The height of the window. Same as the width, if set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
		 */
		explicit Window(Engine& engine, std::string&& title, uint32_t width = -1, uint32_t height = -1);

		/**
		 * Default virtual destructor.
		 */
		virtual ~Window() = default;

		/**
		 * Update the window.
		 * This will end the previous frame (if it's not the first frame), and begin the next frame. The user can update all the resources after this call.
		 */
		virtual void update() = 0;

		/**
		 * Get the engine to which this window is bound to.
		 *
		 * @return The engine reference.
		 */
		[[nodiscard]] Engine& getEngine() { return m_Engine; }

		/**
		 * Get the engine to which this window is bound to.
		 *
		 * @return The const engine reference.
		 */
		[[nodiscard]] const Engine& getEngine() const { return m_Engine; }

		/**
		 * Get the casted engine to which this window is bound to.
		 *
		 * @tparam Type The type to cast to.
		 * @return The type reference.
		 */
		template<class Type>
		[[nodiscard]] Type& getEngineAs() { return *m_Engine.as<Type>(); }

		/**
		 * Get the casted engine to which this window is bound to.
		 *
		 * @tparam Type The type to cast to.
		 * @return The const type reference.
		 */
		template<class Type>
		[[nodiscard]] const Type& getEngineAs() const { return *m_Engine.as<Type>(); }

		/**
		 * Get the title of the window.
		 *
		 * @return The window title.
		 */
		[[nodiscard]] std::string_view getTitle() const { return m_Title; }

		/**
		 * Get the width of the window.
		 *
		 * @return The width.
		 */
		[[nodiscard]] uint32_t getWidth() const { return m_Width; }

		/**
		 * Get the height of the window.
		 *
		 * @return The height.
		 */
		[[nodiscard]] uint32_t getHeight() const { return m_Height; }

	private:
		std::string m_Title;
		Engine& m_Engine;

	protected:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}