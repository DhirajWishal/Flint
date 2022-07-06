// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SpinMutex.hpp"

#include <functional>

namespace Flint
{
	/**
	 * Synchronized class.
	 * This can be used to synchronize a single object.
	 */
	template<class Type>
	class Synchronized
	{
		using ValueType = std::remove_cv_t<Type>;

	public:
		/**
		 * Default constructor.
		 */
		Synchronized() = default;

		/**
		 * Converting constructor.
		 *
		 * @param variable The value to set.
		 */
		Synchronized(const ValueType& variable) : m_Variable(variable) {}

		/**
		 * Converting constructor.
		 *
		 * @param variable The value to set.
		 */
		Synchronized(ValueType&& variable) : m_Variable(std::move(variable)) {}

		/**
		 * Call a function after locking/ synchronizing the internal variable.
		 *
		 * @param callable The callable to be called.
		 */
		void apply(std::function<void(ValueType&)>&& callable) { [[maybe_unused]] auto locker = std::scoped_lock(m_VariableMutex); callable(m_Variable); }

		/**
		 * Call a function on the internal variable.
		 * This will not perform any resource locking.
		 *
		 * @param callable The callable to be called.
		 */
		void apply(std::function<void(const ValueType&)>&& callable) const { callable(m_Variable); }

		/**
		 * Set a value to the variable.
		 *
		 * @param value The value to set.
		 */
		void set(const ValueType& value) { m_Variable = value; }

		/**
		 * Set a value to the variable.
		 *
		 * @param value The value to set.
		 */
		void set(ValueType&& value) { m_Variable = std::move(value); }

		/**
		 * Get the internally stored variable.
		 *
		 * @return The variable.
		 */
		[[nodiscard]] ValueType& get() { return m_Variable; }

		/**
		 * Get the internally stored variable.
		 *
		 * @return The variable.
		 */
		[[nodiscard]] const ValueType& get() const { return m_Variable; }

		/**
		 * Get the internally stored variable's pointer.
		 *
		 * @return The variable pointer.
		 */
		[[nodiscard]] ValueType* pointer() { return &m_Variable; }

		/**
		 * Get the internally stored variable's pointer.
		 *
		 * @return The variable pointer.
		 */
		[[nodiscard]] const ValueType* pointer() const { return &m_Variable; }

		/**
		 * Implicit type cast.
		 *
		 * @return The internally stored variable.
		 */
		[[nodiscard]] operator ValueType& () { return m_Variable; }

		/**
		 * Implicit type cast.
		 *
		 * @return The internally stored variable.
		 */
		[[nodiscard]] operator const ValueType& () const { return m_Variable; }

	private:
		ValueType m_Variable;
		SpinMutex m_VariableMutex;
	};
}