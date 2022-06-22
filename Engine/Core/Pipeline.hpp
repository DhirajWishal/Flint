// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Pipeline class.
		 * This is the base class for all the supported pipelines in the engine.
		 *
		 * Pipelines are used for rendering (ray tracing and raster graphics) and compute operations.
		 */
		class Pipeline : public DeviceBoundObject
		{
		public:
			// Use the same constructor as the super class.
			using DeviceBoundObject::DeviceBoundObject;

			/**
			 * Default virtual destructor.
			 */
			virtual ~Pipeline() = default;
		};
	}
}