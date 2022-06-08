// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Program class.
		 * Flint uses programs to optimize shaders and resources which are bound to them so that we don't need to create unnecessary resources. They are also used to create pipelines.
		 */
		template<class TDevice>
		class Program : public DeviceBoundObject<TDevice>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device reference.
			 */
			explicit Program(TDevice& device) : DeviceBoundObject(device) {}

			/**
			 * Virtual default destructor.
			 */
			virtual ~Program() = default;
		};
	}
}