// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Ray tracer class.
		 * This class performs ray tracing to the bound entities.
		 */
		template<class TDevice>
		class RayTracer : public RenderTarget<TDevice>
		{
		public:
			// We are using the same constructor(s) as the super class.
			using RenderTarget<TDevice>::RenderTarget;

			/**
			 * Virtual default destructor.
			 */
			virtual ~RayTracer() = default;
		};
	}
}