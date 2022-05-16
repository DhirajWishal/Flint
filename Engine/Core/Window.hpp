// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine.hpp"

namespace Flint
{
	/**
	 * Window class.
	 * This contains the render-able window and the event system.
	 */
	class Window : public FObject
	{
	public:
		/**
		 * Default constructor.
		 */
		Window();

		/**
		 * Default virtual destructor.
		 */
		virtual ~Window() = default;

		/**
		 * Update the window.
		 * This will end the previous frame (if it's not the first frame), and begin the next frame. The user can update all the resources after this call.
		 * It will also poll events and dispatch the events to the event handlers.
		 *
		 * @return Boolean value stating if the window is still open.
		 */
		virtual bool update() = 0;

		/**
		 * Get the engine to which this window is bound to.
		 *
		 * @return The engine reference.
		 */
		[[nodiscard]] virtual Engine& getEngine() = 0;

		/**
		 * Get the engine to which this window is bound to.
		 *
		 * @return The const engine reference.
		 */
		[[nodiscard]] virtual const Engine& getEngine() const = 0;
	};

	/**
	 * Window CRTP class.
	 * This class is the static polymorphic class for the Window.
	 */
	template<class TEngine>
	class WindowCRTP : public Window
	{
		static_assert(std::is_base_of_v<Engine, TEngine>, "Invalid engine type! Make sure that the 'TEngine' type is derived from 'Engine'.");

	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine to which this window is bound to.
		 */
		explicit WindowCRTP(TEngine& engine) : m_Engine(engine) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~WindowCRTP() = default;

		/**
		 * Get the actual engine to which this window is bound to.
		 *
		 * @return The engine reference.
		 */
		[[nodiscard]] TEngine& getEngineCRTP() { return m_Engine; }

		/**
		 * Get the actual engine to which this window is bound to.
		 *
		 * @return The const engine reference.
		 */
		[[nodiscard]] const TEngine& getEngineCRTP() const { return m_Engine; }

	private:
		/**
		 * Get the engine to which this window is bound to.
		 *
		 * @return The engine reference.
		 */
		[[nodiscard]] Engine& getEngine() final { return *m_Engine.as<Engine>(); }

		/**
		 * Get the engine to which this window is bound to.
		 *
		 * @return The const engine reference.
		 */
		[[nodiscard]] const Engine& getEngine() const final { return *m_Engine.as<Engine>(); }

	private:
		TEngine& m_Engine;
	};
}