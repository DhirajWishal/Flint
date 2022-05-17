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
		 * Explicit constructor.
		 *
		 * @param engine The engine to which this window is bound to.
		 */
		explicit Window(Engine& engine);

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

	private:
		Engine& m_Engine;
	};
}