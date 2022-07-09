// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

#include <unordered_map>

namespace Flint
{
	/**
	 * Binding map class.
	 * This contains information about all the resource bindings in a program.
	 */
	class BindingMap final
	{
		/**
		 * Binding structure.
		 * This contains the information regarding a single resource binding.
		 */
		struct Binding final
		{
			std::string m_Name;
			uint32_t m_BindingIndex = 0;
			ResourceType m_Type = ResourceType::Undefined;
		};

	public:
		/**
		 * Default constructor.
		 */
		BindingMap() = default;

		/**
		 * Register a new binding.
		 *
		 * @param name The name of the binding.
		 * @param index The binding index.
		 * @param type The type of the binding.
		 */
		void registerBinding(std::string&& name, uint32_t index, ResourceType type) { m_Bindings.emplace_back(std::move(name), index, type); }

		/**
		 * Get the bindings.
		 *
		 * @return The stored bindings.
		 */
		[[nodiscard]] const std::vector<Binding>& getBindings() const { return m_Bindings; }

	private:
		std::vector<Binding> m_Bindings;
	};

	/**
	 * Program class.
	 * Flint uses programs to optimize shaders and resources which are bound to them so that we don't need to create unnecessary resources. They are also used to create pipelines.
	 */
	class Program : public DeviceBoundObject
	{
	public:
		// Using the same constructor as the super class.
		using DeviceBoundObject::DeviceBoundObject;

		/**
		 * Virtual default destructor.
		 */
		virtual ~Program() = default;

		/**
		 * Get the binding map.
		 *
		 * @return The binding map.
		 */
		[[nodiscard]] const BindingMap& getBindingMap() const { return m_BindingMap; }

	protected:
		BindingMap m_BindingMap = {};
	};
}