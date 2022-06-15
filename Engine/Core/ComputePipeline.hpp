// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Compute pipeline class.
		 * This class is used to perform compute operations.
		 */
		template<class TDevice>
		class ComputePipeline : public Pipeline<TDevice>
		{
		public:
			// Using the same constructor as the super class.
			using Pipeline<TDevice>::Pipeline;

			/**
			 * Default virtual destructor.
			 */
			virtual ~ComputePipeline() = default;
		};
	}
}