// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine.hpp"

namespace Flint
{
	/**
	 * Engine bound object class.
	 * This class is the base class for all the engine bound objects.
	 */
	class EngineBoundObject : public FObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine reference.
		 */
		explicit EngineBoundObject(Engine& engine) : m_Engine(engine) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~EngineBoundObject() = default;

		/**
		 * Get the engine to which this object is bound to.
		 *
		 * @return The engine reference.
		 */
		[[nodiscard]] Engine& getEngine() { return m_Engine; }

		/**
		 * Get the engine to which this object is bound to.
		 *
		 * @return The const engine reference.
		 */
		[[nodiscard]] const Engine& getEngine() const { return m_Engine; }

		/**
		 * Get the casted engine to which this object is bound to.
		 *
		 * @tparam Type The type to cast to.
		 * @return The type reference.
		 */
		template<class Type>
		[[nodiscard]] Type& getEngineAs() { return *m_Engine.as<Type>(); }

		/**
		 * Get the casted engine to which this object is bound to.
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