// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Ray tracing pipeline class.
		 * This pipeline is used to perform ray tracing.
		 */
		class RayTracingPipeline : public Pipeline
		{
		public:
			// Using the same constructor as the super class.
			using Pipeline::Pipeline;

			/**
			 * Default virtual destructor.
			 */
			virtual ~RayTracingPipeline() = default;
		};
	}
}