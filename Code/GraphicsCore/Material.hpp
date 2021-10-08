// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "MaterialProperties.hpp"

namespace Flint
{
	/**
	 * Flint material object.
	 */
	class Material final : public FObject
	{
	public:
		Material() = default;

		/**
		 * Add a material property.
		 *
		 * @param pProperty The property to add.
		 */
		void AddProperty(std::shared_ptr<MaterialProperties::Property>&& pProperty) { pProperties.push_back(std::move(pProperty)); }

		/**
		 * Get all the properties.
		 *
		 * @return The properties.
		 */
		const std::vector<std::shared_ptr<MaterialProperties::Property>> GetProperties() const { return pProperties; }

	private:
		std::vector<std::shared_ptr<MaterialProperties::Property>> pProperties = {};
	};
}