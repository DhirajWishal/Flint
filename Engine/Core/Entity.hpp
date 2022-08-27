// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Entity class.
		 * This class is the base class for all the render-able objects, including static models.
		 */
		class Entity : public DeviceBoundObject
		{
		public:
			// We're using the same constructor as the super class.
			using DeviceBoundObject::DeviceBoundObject;

			/**
			 * Default virtual destructor.
			 */
			virtual ~Entity() = default;
		};
	}
}