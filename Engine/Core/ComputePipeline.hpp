// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"

namespace Flint
{
	/**
	 * Compute pipeline class.
	 * This class is used to perform compute operations.
	 */
	class ComputePipeline : public Pipeline
	{
	public:
		// Using the same constructor as the super class.
		using Pipeline::Pipeline;

		/**
		 * Default virtual destructor.
		 */
		virtual ~ComputePipeline() = default;
	};
}