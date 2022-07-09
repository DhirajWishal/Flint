// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>
#include <functional>

namespace Flint
{
	/**
	 * Synchronized class.
	 * This can be used to synchronize a single object.
	 *
	 * Note that the callable should contain an parameter which can be either Type reference, or const Type reference.
	 * 
	 * @tparam Type The type to hold internally.
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
		 * Get the internally stored mutex.
		 *
		 * @return The mutex.
		 */
		[[nodiscard]] std::mutex& getMutex() { return m_VariableMutex; }

		/**
		 * Get the internally stored mutex.
		 *
		 * @return The mutex.
		 */
		[[nodiscard]] const std::mutex& getMutex() const { return m_VariableMutex; }

		/**
		 * Call a function after locking/ synchronizing the internal variable.
		 *
		 * @param callable The callable to be called.
		 */
		template<class Callable>
		decltype(auto) apply(Callable&& callable) { [[maybe_unused]] auto locker = std::scoped_lock(m_VariableMutex); return callable(m_Variable); }

		/**
		 * Call a function on the internal variable.
		 * This will not perform any resource locking.
		 *
		 * @param callable The callable to be called.
		 */
		template<class Callable>
		decltype(auto) apply(Callable&& callable) const { return callable(m_Variable); }

		/**
		 * Set a value to the variable.
		 *
		 * @param value The value to set.
		 */
		void set(const ValueType& value) { [[maybe_unused]] auto locker = std::scoped_lock(m_VariableMutex); m_Variable = value; }

		/**
		 * Set a value to the variable.
		 *
		 * @param value The value to set.
		 */
		void set(ValueType&& value) { [[maybe_unused]] auto locker = std::scoped_lock(m_VariableMutex); m_Variable = std::move(value); }

		/**
		 * Get the internally stored variable.
		 * Note that this operation is unsafe, meaning that it does not do any resource locking.
		 *
		 * @return The variable.
		 */
		[[nodiscard]] ValueType& getUnsafe() { return m_Variable; }

		/**
		 * Get the internally stored variable.
		 * Note that this operation is unsafe, meaning that it does not do any resource locking.
		 *
		 * @return The variable.
		 */
		[[nodiscard]] const ValueType& getUnsafe() const { return m_Variable; }

		/**
		 * Get the internally stored variable's pointer.
		 * Note that this operation is unsafe, meaning that it does not do any resource locking.
		 *
		 * @return The variable pointer.
		 */
		[[nodiscard]] ValueType* pointerUnsafe() { return &m_Variable; }

		/**
		 * Get the internally stored variable's pointer.
		 * Note that this operation is unsafe, meaning that it does not do any resource locking.
		 *
		 * @return The variable pointer.
		 */
		[[nodiscard]] const ValueType* pointerUnsafe() const { return &m_Variable; }

		/**
		 * Value assignment operator.
		 *
		 * @param value The value to assign.
		 * @return This object reference.
		 */
		Synchronized& operator=(const ValueType& value) { [[maybe_unused]] auto locker = std::scoped_lock(m_VariableMutex); m_Variable = value; return *this; }

		/**
		 * Value assignment operator.
		 *
		 * @param value The value to assign.
		 * @return This object reference.
		 */
		Synchronized& operator=(ValueType&& value) { [[maybe_unused]] auto locker = std::scoped_lock(m_VariableMutex); m_Variable = std::move(value); return *this; }

	private:
		ValueType m_Variable;
		std::mutex m_VariableMutex;
	};
}