// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

namespace Flint
{
	/**
	 * Engine class.
	 * This class contains everything that's needed for a single engine instance.
	 *
	 * Usually an application requires just one instance of this class. But the user isn't restricted to do that in any way. But make sure that the engines are destroyed before
	 * destroying the instance which is used to create this.
	 */
	class Engine : public FObject
	{
	public:
		/**
		 * Default constructor.
		 */
		constexpr Engine() = default;

		/**
		 * Default virtual destructor.
		 */
		virtual ~Engine() = default;

		/**
		 * Get the instance.
		 *
		 * @return The instance reference.
		 */
		[[nodiscard]] virtual Instance& getInstance() = 0;

		/**
		 * Get the instance.
		 *
		 * @return The const instance reference.
		 */
		[[nodiscard]] virtual const Instance& getInstance() const = 0;
	};

	/**
	 * Engine CRTP class.
	 * This class is the static polymorphic type of the engine and the actual backend class is inherited using this.
	 *
	 * @tparam TInstance The instance type.
	 */
	template<class TInstance>
	class EngineCRTP : public Engine
	{
		static_assert(std::is_base_of_v<Instance, TInstance>, "Invalid instance type! Make sure that the 'TInstance' type is derived from 'Instance'.");

	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance to create the engine with.
		 */
		explicit EngineCRTP(TInstance& instance) : m_Instance(instance) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~EngineCRTP() = default;

		/**
		 * Get the actual instance.
		 *
		 * @return The instance reference.
		 */
		[[nodiscard]] TInstance& getInstanceCRTP() { return m_Instance; }

		/**
		 * Get the actual instance.
		 *
		 * @return The const instance reference.
		 */
		[[nodiscard]] const TInstance& getInstanceCRTP() const { return m_Instance; }

	private:
		/**
		 * Get the instance.
		 *
		 * @return The instance reference.
		 */
		[[nodiscard]] Instance& getInstance() final { return *(m_Instance.as<Instance>()); }

		/**
		 * Get the instance.
		 *
		 * @return The const instance reference.
		 */
		[[nodiscard]] const Instance& getInstance() const final { return *(m_Instance.as<Instance>()); }

	private:
		TInstance& m_Instance;
	};
}