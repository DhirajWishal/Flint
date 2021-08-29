// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "MaterialProperties.hpp"

namespace Flint
{
	/**
	 * Flint material object.
	 */
	class Material : public FObject
	{
	public:
		Material() {}

		void AddProperty(std::unique_ptr<MaterialProperties::Property>&& pProperty) { pProperties.push_back(std::move(pProperty)); }

	private:
		boost::container::vector<std::unique_ptr<MaterialProperties::Property>> pProperties = {};
	};
}